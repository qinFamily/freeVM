/*
 *  Copyright 2005-2006 The Apache Software Foundation or its licensors, as applicable.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * @author Xiao-Feng Li, 2006/10/05
 */

#ifndef _GC_COMMON_H_
#define _GC_COMMON_H_

#include "port_vmem.h"

#include "platform_lowlevel.h"

#include "open/types.h"
#include "open/vm_gc.h"
#include "open/vm.h"
#include "open/gc.h"
#include "port_malloc.h"

#include "gc_for_class.h"
#include "gc_platform.h"

#include "../gen/gc_for_barrier.h"

#define null 0

#define KB  (1<<10)
#define MB  (1<<20)

#define BITS_PER_BYTE 8 
#define BYTES_PER_WORD (sizeof(POINTER_SIZE_INT))
#define BITS_PER_WORD (BITS_PER_BYTE * BYTES_PER_WORD)


#define MASK_OF_BYTES_PER_WORD (BYTES_PER_WORD-1) /* 0x11 */

#define BIT_SHIFT_TO_BITS_PER_BYTE 3

#ifdef POINTER64
  #define BIT_SHIFT_TO_BYTES_PER_WORD 3 /* 3 */
#else
  #define BIT_SHIFT_TO_BYTES_PER_WORD 2 /* 2 */
#endif

#ifdef POINTER64
  #define BIT_SHIFT_TO_BITS_PER_WORD 6
#else
  #define BIT_SHIFT_TO_BITS_PER_WORD 5
#endif

#define BIT_SHIFT_TO_KILO 10 
#define BIT_MASK_TO_BITS_PER_WORD ((1<<BIT_SHIFT_TO_BITS_PER_WORD)-1)
#define BITS_OF_POINTER_SIZE_INT (sizeof(POINTER_SIZE_INT) << BIT_SHIFT_TO_BITS_PER_BYTE)
#define BYTES_OF_POINTER_SIZE_INT (sizeof(POINTER_SIZE_INT))
#define BIT_SHIFT_TO_BYTES_OF_POINTER_SIZE_INT ((sizeof(POINTER_SIZE_INT)==4)? 2: 3)

#define GC_OBJ_SIZE_THRESHOLD (5*KB)

#define USE_32BITS_HASHCODE

//#define ONLY_SSPACE_IN_HEAP

typedef void (*TaskType)(void*);

enum Collection_Algorithm{
  COLLECTION_ALGOR_NIL,
  
  /*minor nongen collection*/
  MINOR_NONGEN_FORWARD_POOL,
  
  /* minor gen collection */
  MINOR_GEN_FORWARD_POOL,
  
  /* major collection */
  MAJOR_COMPACT_SLIDE,
  MAJOR_COMPACT_MOVE
  
};

/* Possible combinations:
 * MINOR_COLLECTION
 * MAJOR_COLLECTION
 * FALLBACK_COLLECTION
 * MAJOR_COLLECTION | EXTEND_COLLECTION
 * FALLBACK_COLLECTION | EXTEND_COLLECTION
 */
enum Collection_Kind {
  MINOR_COLLECTION = 0x1,
  MAJOR_COLLECTION = 0x2,
  FALLBACK_COLLECTION = 0x4,
  EXTEND_COLLECTION = 0x8,
  UNIQUE_SWEEP_COLLECTION = 0x10
};

extern Boolean IS_FALLBACK_COMPACTION;  /* only for mark/fw bits debugging purpose */

enum GC_CAUSE{
  GC_CAUSE_NIL,
  GC_CAUSE_NOS_IS_FULL,
  GC_CAUSE_LOS_IS_FULL,
  GC_CAUSE_POS_IS_FULL,
  GC_CAUSE_RUNTIME_FORCE_GC
};

/*Fixme: There is only compressed mode under em64t currently.*/
#ifdef POINTER64
  #define COMPRESS_REFERENCE
#endif

extern POINTER_SIZE_INT HEAP_NULL;

#ifdef POINTER64
  #ifdef COMPRESS_REFERENCE
    #define REF uint32
  #else
    #define REF Partial_Reveal_Object*
  #endif
#else/*ifdef POINTER64*/
  #define REF Partial_Reveal_Object*
#endif

/////////////////////////////////////////////
//Compress reference related!///////////////////
/////////////////////////////////////////////
FORCE_INLINE REF obj_ptr_to_ref(Partial_Reveal_Object *p_obj)
{
#ifdef COMPRESS_REFERENCE
  if(!p_obj){
	  /*Fixme: em64t: vm performs a simple compress/uncompress machenism
	   i.e. just add or minus HEAP_NULL to p_obj
	   But in gc we distinguish zero from other p_obj
	   Now only in prefetch next live object we can hit this point. */
    return (REF)0;
  }
  else
    return (REF) ((POINTER_SIZE_INT) p_obj - HEAP_NULL);
#else

	return (REF)p_obj;

#endif

}

FORCE_INLINE Partial_Reveal_Object *ref_to_obj_ptr(REF ref)
{
#ifdef COMPRESS_REFERENCE
  if(!ref){
    return NULL; 
  }
  return (Partial_Reveal_Object *)(HEAP_NULL + ref);

#else

	return (Partial_Reveal_Object *)ref;

#endif

}

FORCE_INLINE Partial_Reveal_Object *read_slot(REF *p_slot)
{  return ref_to_obj_ptr(*p_slot); }

FORCE_INLINE void write_slot(REF *p_slot, Partial_Reveal_Object *p_obj)
{  *p_slot = obj_ptr_to_ref(p_obj); }


inline POINTER_SIZE_INT round_up_to_size(POINTER_SIZE_INT size, int block_size) 
{  return (size + block_size - 1) & ~(block_size - 1); }

inline POINTER_SIZE_INT round_down_to_size(POINTER_SIZE_INT size, int block_size) 
{  return size & ~(block_size - 1); }

/****************************************/
/* Return a pointer to the ref field offset array. */
inline int* object_ref_iterator_init(Partial_Reveal_Object *obj)
{
  GC_VTable_Info *gcvt = obj_get_gcvt(obj);  
  return gcvt->gc_ref_offset_array;    
}

inline REF* object_ref_iterator_get(int* iterator, Partial_Reveal_Object *obj)
{
  return (REF*)((POINTER_SIZE_INT)obj + *iterator);
}

inline int* object_ref_iterator_next(int* iterator)
{
  return iterator+1;
}

/* original design */
inline int *init_object_scanner (Partial_Reveal_Object *obj) 
{
  GC_VTable_Info *gcvt = obj_get_gcvt(obj);  
  return gcvt->gc_ref_offset_array;
}

inline void *offset_get_ref(int *offset, Partial_Reveal_Object *obj) 
{    return (*offset == 0)? NULL: (void*)((Byte*) obj + *offset); }

inline int *offset_next_ref (int *offset) 
{  return offset + 1; }

/****************************************/

inline Boolean obj_is_marked_in_vt(Partial_Reveal_Object *obj) 
{  return (Boolean)((POINTER_SIZE_INT)obj_get_vt_raw(obj) & CONST_MARK_BIT); }

inline Boolean obj_mark_in_vt(Partial_Reveal_Object *obj) 
{  
  VT vt = obj_get_vt_raw(obj);
  if((POINTER_SIZE_INT)vt & CONST_MARK_BIT) return FALSE;
  obj_set_vt(obj,  (VT)( (POINTER_SIZE_INT)vt | CONST_MARK_BIT ) );
  return TRUE;
}

inline void obj_unmark_in_vt(Partial_Reveal_Object *obj) 
{ 
  VT vt = obj_get_vt_raw(obj);
  obj_set_vt(obj, (VT)((POINTER_SIZE_INT)vt & ~CONST_MARK_BIT));
}

inline void obj_clear_dual_bits_in_vt(Partial_Reveal_Object* p_obj){
  VT vt = obj_get_vt_raw(p_obj);
  obj_set_vt(p_obj,(VT)((POINTER_SIZE_INT)vt & DUAL_MARKBITS_MASK));
}

inline Boolean obj_is_marked_or_fw_in_oi(Partial_Reveal_Object *obj)
{ return get_obj_info_raw(obj) & DUAL_MARKBITS; }


inline void obj_clear_dual_bits_in_oi(Partial_Reveal_Object *obj)
{  
  Obj_Info_Type info = get_obj_info_raw(obj);
  set_obj_info(obj, (unsigned int)info & DUAL_MARKBITS_MASK);
}

/****************************************/
#ifndef MARK_BIT_FLIPPING

inline Partial_Reveal_Object *obj_get_fw_in_oi(Partial_Reveal_Object *obj) 
{
  assert(get_obj_info_raw(obj) & CONST_FORWARD_BIT);
  return (Partial_Reveal_Object*)(ref_to_obj_ptr((REF)(get_obj_info_raw(obj) & ~CONST_FORWARD_BIT)));
}

inline Boolean obj_is_fw_in_oi(Partial_Reveal_Object *obj) 
{  return (get_obj_info_raw(obj) & CONST_FORWARD_BIT); }

inline void obj_set_fw_in_oi(Partial_Reveal_Object *obj,void *dest)
{  
  assert(!(get_obj_info_raw(obj) & CONST_FORWARD_BIT));
  REF dest = obj_ptr_to_ref((Partial_Reveal_Object *) dest);
  set_obj_info(obj,(Obj_Info_Type)dest | CONST_FORWARD_BIT); 
}


inline Boolean obj_is_marked_in_oi(Partial_Reveal_Object *obj) 
{  return ( get_obj_info_raw(obj) & CONST_MARK_BIT ); }

inline Boolean obj_mark_in_oi(Partial_Reveal_Object *obj) 
{  
  Obj_Info_Type info = get_obj_info_raw(obj);
  if ( info & CONST_MARK_BIT ) return FALSE;

  set_obj_info(obj, info|CONST_MARK_BIT);
  return TRUE;
}

inline void obj_unmark_in_oi(Partial_Reveal_Object *obj) 
{  
  Obj_Info_Type info = get_obj_info_raw(obj);
  info = info & ~CONST_MARK_BIT;
  set_obj_info(obj, info);
  return;
}

/* **********************************  */
#else /* ifndef MARK_BIT_FLIPPING */

inline void mark_bit_flip()
{ 
  FLIP_FORWARD_BIT = FLIP_MARK_BIT;
  FLIP_MARK_BIT ^= DUAL_MARKBITS; 
}

inline Partial_Reveal_Object *obj_get_fw_in_oi(Partial_Reveal_Object *obj) 
{
  assert(get_obj_info_raw(obj) & FLIP_FORWARD_BIT);
  return (Partial_Reveal_Object*) ( ref_to_obj_ptr( (REF)get_obj_info(obj) ) );
}

inline Boolean obj_is_fw_in_oi(Partial_Reveal_Object *obj) 
{  return (get_obj_info_raw(obj) & FLIP_FORWARD_BIT); }

inline void obj_set_fw_in_oi(Partial_Reveal_Object *obj, void *dest)
{ 
  assert(IS_FALLBACK_COMPACTION || (!(get_obj_info_raw(obj) & FLIP_FORWARD_BIT))); 
  /* This assert should always exist except it's fall back compaction. In fall-back compaction
     an object can be marked in last time minor collection, which is exactly this time's fw bit,
     because the failed minor collection flipped the bits. */

  /* It's important to clear the FLIP_FORWARD_BIT before collection ends, since it is the same as
     next minor cycle's FLIP_MARK_BIT. And if next cycle is major, it is also confusing
     as FLIP_FORWARD_BIT. (The bits are flipped only in minor collection). */
  Obj_Info_Type dst = (Obj_Info_Type)obj_ptr_to_ref((Partial_Reveal_Object *) dest);     
  set_obj_info(obj, dst | FLIP_FORWARD_BIT); 
}

inline Boolean obj_mark_in_oi(Partial_Reveal_Object* p_obj)
{
  Obj_Info_Type info = get_obj_info_raw(p_obj);
  assert((info & DUAL_MARKBITS ) != DUAL_MARKBITS);
  
  if( info & FLIP_MARK_BIT ) return FALSE;  
  
  info = info & DUAL_MARKBITS_MASK;
  set_obj_info(p_obj, info|FLIP_MARK_BIT);
  return TRUE;
}

inline Boolean obj_unmark_in_oi(Partial_Reveal_Object* p_obj)
{
  Obj_Info_Type info = get_obj_info_raw(p_obj);
  info = info & DUAL_MARKBITS_MASK;
  set_obj_info(p_obj, info);
  return TRUE;
}

inline Boolean obj_is_marked_in_oi(Partial_Reveal_Object* p_obj)
{
  Obj_Info_Type info = get_obj_info_raw(p_obj);
  return (info & FLIP_MARK_BIT);
}

#endif /* MARK_BIT_FLIPPING */

/* all GCs inherit this GC structure */
struct Mutator;
struct Collector;
struct GC_Metadata;
struct Finref_Metadata;
struct Vector_Block;
struct Space_Tuner;

typedef struct GC{
  void* heap_start;
  void* heap_end;
  POINTER_SIZE_INT reserved_heap_size;
  POINTER_SIZE_INT committed_heap_size;
  unsigned int num_collections;
  int64 time_collections;
  float survive_ratio;
  
  /* mutation related info */
  Mutator *mutator_list;
  SpinLock mutator_list_lock;
  unsigned int num_mutators;

  /* collection related info */    
  Collector** collectors;
  unsigned int num_collectors;
  unsigned int num_active_collectors; /* not all collectors are working */
  
  /* metadata is the pool for rootset, tracestack, etc. */  
  GC_Metadata* metadata;
  Finref_Metadata *finref_metadata;

  unsigned int collect_kind; /* MAJOR or MINOR */
  unsigned int last_collect_kind;
  unsigned int cause;/*GC_CAUSE_LOS_IS_FULL, GC_CAUSE_NOS_IS_FULL, or GC_CAUSE_RUNTIME_FORCE_GC*/
  Boolean collect_result; /* succeed or fail */

  Boolean generate_barrier;
  
  /* FIXME:: this is wrong! root_set belongs to mutator */
  Vector_Block* root_set;
  Vector_Block* uncompressed_root_set;

  Space_Tuner* tuner;

}GC;

void mark_scan_pool(Collector* collector);

inline void mark_scan_heap(Collector* collector)
{
    mark_scan_pool(collector);    
}

inline void* gc_heap_base(GC* gc){ return gc->heap_start; }
inline void* gc_heap_ceiling(GC* gc){ return gc->heap_end; }
inline Boolean address_belongs_to_gc_heap(void* addr, GC* gc)
{
  return (addr >= gc_heap_base(gc) && addr < gc_heap_ceiling(gc));
}

inline Boolean gc_match_kind(GC *gc, unsigned int kind)
{
  assert(gc->collect_kind && kind);
  return gc->collect_kind & kind;
}

void gc_parse_options(GC* gc);
void gc_reclaim_heap(GC* gc, unsigned int gc_cause);

/* generational GC related */

extern Boolean NOS_PARTIAL_FORWARD;

//#define STATIC_NOS_MAPPING

#ifdef STATIC_NOS_MAPPING

  //#define NOS_BOUNDARY ((void*)0x2ea20000)  //this is for 512M
  #define NOS_BOUNDARY ((void*)0x40000000) //this is for 256M

	#define nos_boundary NOS_BOUNDARY

#else /* STATIC_NOS_MAPPING */

	extern void* nos_boundary;

#endif /* STATIC_NOS_MAPPING */

inline Boolean addr_belongs_to_nos(void* addr)
{ return addr >= nos_boundary; }

inline Boolean obj_belongs_to_nos(Partial_Reveal_Object* p_obj)
{ return addr_belongs_to_nos(p_obj); }

extern void* los_boundary;

/*This flag indicate whether lspace is using a sliding compaction
 *Fixme: check if the performance is a problem with this global flag.
 */
extern Boolean* p_global_lspace_move_obj;
inline Boolean obj_is_moved(Partial_Reveal_Object* p_obj)
{  return ((p_obj >= los_boundary) || (*p_global_lspace_move_obj)); }
#endif //_GC_COMMON_H_
