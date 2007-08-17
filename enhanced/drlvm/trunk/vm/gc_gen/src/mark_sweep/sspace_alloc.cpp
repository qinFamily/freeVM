/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "sspace.h"
#include "sspace_chunk.h"
//#include "sspace_mark_sweep.h"
#include "sspace_alloc.h"
#include "gc_ms.h"
#include "../gen/gen.h"


/* Only used in pfc_set_slot_index() */
inline unsigned int first_free_index_in_color_word(POINTER_SIZE_INT word, POINTER_SIZE_INT alloc_color)
{
  for(unsigned int index = 0; index < BITS_PER_WORD; index += COLOR_BITS_PER_OBJ)
    if(!(word & (alloc_color << index)))
      return index;
  
  assert(0);  /* There must be a free obj in this table word */
  return MAX_SLOT_INDEX;
}

/* Given an index word in table, set pfc's slot_index
 * The value of argument alloc_color can be cur_alloc_color or cur_mark_color.
 * It depends on in which phase this func is called.
 * In sweeping phase, sspace has been marked but alloc and mark colors have not been flipped,
 * so we have to use cur_mark_color as alloc_color.
 * In compaction phase, two colors have been flipped, so we use cur_alloc_color.
 */
void pfc_set_slot_index(Chunk_Header *chunk, unsigned int first_free_word_index, POINTER_SIZE_INT alloc_color)
{
  unsigned int index_in_word = first_free_index_in_color_word(chunk->table[first_free_word_index], alloc_color);
  assert(index_in_word != MAX_SLOT_INDEX);
  chunk->slot_index = composed_slot_index(first_free_word_index, index_in_word);
}

/* From the table's beginning search the first free slot, and set it to pfc's slot_index */
void pfc_reset_slot_index(Chunk_Header *chunk)
{
  POINTER_SIZE_INT *table = chunk->table;
  
  unsigned int index_word_num = (chunk->slot_num + SLOT_NUM_PER_WORD_IN_TABLE - 1) / SLOT_NUM_PER_WORD_IN_TABLE;
  for(unsigned int i=0; i<index_word_num; ++i){
    if(table[i] != cur_alloc_mask){
      pfc_set_slot_index(chunk, i, cur_alloc_color);
      return;
    }
  }
}

/* Alloc small without-fin object in sspace without getting new free chunk */
void *sspace_thread_local_alloc(unsigned size, Allocator *allocator)
{
  if(size > LARGE_OBJ_THRESHOLD) return NULL;
  
  Sspace *sspace = gc_get_sspace(allocator->gc);
  
  /* Flexible alloc mechanism:
  Size_Segment *size_seg = sspace_get_size_seg(sspace, size);
  unsigned int seg_index = size_seg->seg_index;
  */
  unsigned int seg_index = (size-GC_OBJECT_ALIGNMENT) / MEDIUM_OBJ_THRESHOLD;
  assert(seg_index <= 2);
  Size_Segment *size_seg = sspace->size_segments[seg_index];
  assert(size_seg->local_alloc);
  
  size = (unsigned int)NORMAL_SIZE_ROUNDUP(size, size_seg);
  unsigned int index = NORMAL_SIZE_TO_INDEX(size, size_seg);
  
  Chunk_Header **chunks = allocator->local_chunks[seg_index];
  Chunk_Header *chunk = chunks[index];
  if(!chunk){
    chunk = sspace_get_pfc(sspace, seg_index, index);
    //if(!chunk) chunk = sspace_steal_pfc(sspace, seg_index, index);
    if(!chunk) return NULL;
    chunk->status |= CHUNK_IN_USE;
    chunks[index] = chunk;
  }
  void *p_obj = alloc_in_chunk(chunks[index]);
  assert(p_obj);

#ifdef SSPACE_ALLOC_INFO
  sspace_alloc_info(size);
#endif
#ifdef SSPACE_VERIFY
  sspace_verify_alloc(p_obj, size);
#endif

  return p_obj;
}

static void *sspace_alloc_normal_obj(Sspace *sspace, unsigned size, Allocator *allocator)
{
  Size_Segment *size_seg = sspace_get_size_seg(sspace, size);
  unsigned int seg_index = size_seg->seg_index;
  
  size = (unsigned int)NORMAL_SIZE_ROUNDUP(size, size_seg);
  unsigned int index = NORMAL_SIZE_TO_INDEX(size, size_seg);
  
  Chunk_Header *chunk = NULL;
  void *p_obj = NULL;
  
  if(size_seg->local_alloc){
    Chunk_Header **chunks = allocator->local_chunks[seg_index];
    chunk = chunks[index];
    if(!chunk){
      chunk = sspace_get_pfc(sspace, seg_index, index);
      if(!chunk){
        chunk = (Chunk_Header*)sspace_get_normal_free_chunk(sspace);
        if(chunk) normal_chunk_init(chunk, size);
      }
      //if(!chunk) chunk = sspace_steal_pfc(sspace, seg_index, index);
      if(!chunk) return NULL;
      chunk->status |= CHUNK_IN_USE;
      chunks[index] = chunk;
    }
    p_obj = alloc_in_chunk(chunks[index]);
  } else {
    chunk = sspace_get_pfc(sspace, seg_index, index);
    if(!chunk){
      chunk = (Chunk_Header*)sspace_get_normal_free_chunk(sspace);
      if(chunk) normal_chunk_init(chunk, size);
    }
    //if(!chunk) chunk = sspace_steal_pfc(sspace, seg_index, index);
    if(!chunk) return NULL;
    assert(chunk->alloc_num < chunk->slot_num);
    ++chunk->alloc_num;
    p_obj = alloc_in_chunk(chunk);
    if(chunk)
      sspace_put_pfc(sspace, chunk);
  }
  
  return p_obj;
}

static void *sspace_alloc_super_obj(Sspace *sspace, unsigned size, Allocator *allocator)
{
  assert(size > SUPER_OBJ_THRESHOLD);

  unsigned int chunk_size = SUPER_SIZE_ROUNDUP(size);
  assert(chunk_size > SUPER_OBJ_THRESHOLD);
  assert(!(chunk_size & CHUNK_GRANULARITY_LOW_MASK));
  
  Chunk_Header *chunk;
  if(chunk_size <= HYPER_OBJ_THRESHOLD)
    chunk = (Chunk_Header*)sspace_get_abnormal_free_chunk(sspace, chunk_size);
  else
    chunk = (Chunk_Header*)sspace_get_hyper_free_chunk(sspace, chunk_size, FALSE);
  
  if(!chunk) return NULL;
  abnormal_chunk_init(chunk, chunk_size, size);
  chunk->table[0] = cur_alloc_color;
  set_super_obj_mask(chunk->base);
  assert(get_obj_info_raw((Partial_Reveal_Object*)chunk->base) & SUPER_OBJ_MASK);
  //printf("Obj: %x  size: %x\t", (POINTER_SIZE_INT)chunk->base, size);
  return chunk->base;
}

static void *sspace_try_alloc(unsigned size, Allocator *allocator)
{
  Sspace *sspace = gc_get_sspace(allocator->gc);
  void *p_obj = NULL;
  
  if(size <= SUPER_OBJ_THRESHOLD)
    p_obj = sspace_alloc_normal_obj(sspace, size, allocator);
  else
    p_obj = sspace_alloc_super_obj(sspace, size, allocator);

#ifdef SSPACE_ALLOC_INFO
  if(p_obj) sspace_alloc_info(size);
#endif
#ifdef SSPACE_VERIFY
  if(p_obj) sspace_verify_alloc(p_obj, size);
#endif

  return p_obj;
}

/* FIXME:: the collection should be seperated from the alloation */
void *sspace_alloc(unsigned size, Allocator *allocator)
{
  void *p_obj = NULL;
  
  /* First, try to allocate object from TLB (thread local chunk) */
  p_obj = sspace_try_alloc(size, allocator);
  if(p_obj)  return p_obj;
  
  vm_gc_lock_enum();
  /* after holding lock, try if other thread collected already */
  p_obj = sspace_try_alloc(size, allocator);
  if(p_obj){
    vm_gc_unlock_enum();
    return p_obj;
  }
  gc_reclaim_heap(allocator->gc, GC_CAUSE_POS_IS_FULL);
  vm_gc_unlock_enum();

#ifdef SSPACE_CHUNK_INFO
  printf("Failure size: %x\n", size);
#endif

  p_obj = sspace_try_alloc(size, allocator);
  
  return p_obj;
}
