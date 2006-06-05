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
 * @author Intel, Evgueni Brevnov, Ivan Volosyuk
 * @version $Revision: 1.1.2.2.4.5 $
 */  


//MVM
#include <iostream>

using namespace std;

#include <assert.h>

#define LOG_DOMAIN "vm.helpers"
#include "cxxlog.h"

#include "object_layout.h"
#include "open/types.h"
#include "Class.h"
#include "environment.h"
#include "lil.h"
#include "lil_code_generator.h"
#include "method_lookup.h"
#include "exceptions.h"
#include "vm_synch.h"
#include "open/gc.h"
#include "ini.h"
#include "nogc.h"
#include "encoder.h"
#include "open/vm_util.h"
#include "open/thread.h"
#include "vm_threads.h"
#include "mon_enter_exit.h"
#include "vm_arrays.h"
#include "vm_strings.h"
#include "compile.h"

#include "mon_enter_exit.h"

#include "sync_bits.h"

#include "vm_stats.h"
#include "internal_jit_intf.h"
#include "jit_runtime_support_common.h"
#include "jit_runtime_support.h"

#include "../m2n_ia32_internal.h"

#include "open/vm_util.h"

#ifndef NDEBUG
#include "dump.h"
#endif
// TODO: remove from global name space
extern bool dump_stubs;

// gets the offset of a certain field within a struct or class type
#define OFFSET(Struct, Field) \
  ((int) (&(((Struct *) NULL)->Field) - NULL))

// gets the size of a field within a struct or class
#define SIZE(Struct, Field) \
  (sizeof(((Struct *) NULL)->Field))


void * getaddress__vm_monitor_enter_naked();
void * getaddress__vm_monitor_enter_static_naked();
void * getaddress__vm_monitor_exit_naked();
void * getaddress__vm_monitor_exit_static_naked();
void * get_generic_rt_support_addr_ia32(VM_RT_SUPPORT f);


/////////////////////////////////////////////////////////////////
// begin VM_Runtime_Support
/////////////////////////////////////////////////////////////////
CriticalSection cs;

static void vm_throw_java_lang_ClassCastException()
{
    assert(!tmn_is_suspend_enabled());
    throw_java_exception("java/lang/ClassCastException");
    ABORT("The last called function should not return");
} //vm_throw_java_lang_ClassCastException

#ifdef VM_STATS // exclude remark in release mode (defined but not used)
static void update_checkcast_stats(ManagedObject *obj, Class *c)
{
    vm_stats_total.num_checkcast ++;
    if (obj == NULL)
        vm_stats_total.num_checkcast_null ++;
    if (obj != NULL && obj->vt()->clss == c)
        vm_stats_total.num_checkcast_equal_type ++;
    if (obj != NULL && c->is_suitable_for_fast_instanceof)
        vm_stats_total.num_checkcast_fast_decision ++;
} //update_checkcast_stats
#endif

// 20030321 This JIT support routine expects to be called directly from managed code.
// NOTE: We do not translate null references since vm_instanceof() also expects to be 
//       called directly by managed code.
static void *getaddress__vm_checkcast_naked()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }
    if (VM_Global_State::loader_env->use_lil_stubs) {
        LilCodeStub *cs = gen_lil_typecheck_stub(true);
        addr = LilCodeGenerator::get_platform()->compile(cs, "vm_rt_checkcast", dump_stubs);
        lil_free_code_stub(cs);
        return addr;
    }
    const int stub_size = (VM_Global_State::loader_env->compress_references? 82 : 58);
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_COLD, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;

#ifdef VM_STATS
    ss = push(ss,  M_Base_Opnd(esp_reg, 8));
    ss = push(ss,  M_Base_Opnd(esp_reg, 8));
    ss = call(ss, (char *)update_checkcast_stats);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(8));
#endif // VM_STATS

    ss = mov(ss,  eax_opnd,  M_Base_Opnd(esp_reg, +4) );
    if (VM_Global_State::loader_env->compress_references) {
        ss = alu(ss, cmp_opc,  eax_opnd,  Imm_Opnd((unsigned)Class::heap_base)); //is eax == NULL?
        ss = branch8(ss, Condition_NZ,  Imm_Opnd(size_8, 0));
    } else {
        ss = test(ss,  eax_opnd,  eax_opnd);
        ss = branch8(ss, Condition_NE,  Imm_Opnd(size_8, 0));  // will get backpatched
    }
    char *backpatch_address__not_null = ((char *)ss) - 1;

    // The object reference is null, so return null.     
    ss = ret(ss,  Imm_Opnd(8));

    // Non-null object reference. Call instanceof.
    signed offset = (signed)ss - (signed)backpatch_address__not_null - 1;
    *backpatch_address__not_null = (char)offset;
    ss = push(ss,  M_Base_Opnd(esp_reg, 8));
    ss = push(ss,  eax_opnd);
    // 20030321 If compressing references, no need to convert a null reference before calling the instanceof JIT support routine
    // since the object is known to be non-null.
    ss = call(ss, (char *)vm_instanceof);

    ss = test(ss,  eax_opnd,  eax_opnd);
    ss = branch8(ss, Condition_E,  Imm_Opnd(size_8, 0));  // will get backpatched
    char *backpatch_address__instanceof_failed = ((char *)ss) - 1;
    ss = mov(ss,  eax_opnd,  M_Base_Opnd(esp_reg, +4) );
    // 20030321 If compressing references, no need to convert a null reference before returning since the object is known to be non-null.
    ss = ret(ss,  Imm_Opnd(8));

    // The checkcast failed so throw java_lang_ClassCastException.
    offset = (signed)ss - (signed)backpatch_address__instanceof_failed - 1;
    *backpatch_address__instanceof_failed = (char)offset;

    ss = gen_setup_j2n_frame(ss);
    ss = call(ss, (char *)vm_throw_java_lang_ClassCastException);

    addr = stub;
    assert((ss - stub) <= stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_rt_checkcast", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_rt_checkcast", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__vm_checkcast_naked", ss - stub);
#endif
    return addr;
}  //getaddress__vm_checkcast_naked




// The code is added to this function so that we can have a LIL version of
// instanceof.  If LIL is turned off, the address of vm_instanceof is
// returned, just like before.
static void *getaddress__vm_instanceof()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    if (VM_Global_State::loader_env->use_lil_stubs)
    {
        LilCodeStub *cs = gen_lil_typecheck_stub(false);
        assert(lil_is_valid(cs));
        addr = LilCodeGenerator::get_platform()->compile(cs, "vm_instanceof", dump_stubs);
        lil_free_code_stub(cs);
        return addr;
    }

    // just use vm_instanceof
    addr = (void *) vm_instanceof;
    return addr;
}


static Boolean is_class_initialized(Class *clss)
{
#ifdef VM_STATS
    vm_stats_total.num_is_class_initialized++;
    clss->num_class_init_checks++;
#endif // VM_STATS
    assert(!tmn_is_suspend_enabled());
    return clss->state == ST_Initialized;
} //is_class_initialized



static void *getaddress__vm_initialize_class_naked()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    if (VM_Global_State::loader_env->use_lil_stubs) {
        LilCodeStub* cs = lil_parse_code_stub(
            "entry 0:managed:pint:void;   // The single argument is a Class_Handle \n"
            "in2out platform:pint; \
             call %0i; \
             jc r=0,not_initialized; \
             ret; \
             :not_initialized; \
             push_m2n 0, 0; \
             in2out platform:void; \
             call %1i; \
             pop_m2n; \
             ret;",
            (void *)is_class_initialized,
            (void *)class_initialize);
        assert(lil_is_valid(cs));
        addr = LilCodeGenerator::get_platform()->compile(cs, "vm_initialize_class_naked", dump_stubs);
        lil_free_code_stub(cs);
        return addr;
    }

    const int stub_size = 44;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_COLD, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;

    ss = push(ss,  M_Base_Opnd(esp_reg, 4));        // push the argument, a ClassHandle
    ss = call(ss, (char *)is_class_initialized);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(4)); // pop the argument since is_class_initialized() is a CDecl function

    ss = test(ss,  eax_opnd,  eax_opnd);
    ss = branch8(ss, Condition_Z,  Imm_Opnd(size_8, 0));
    char *backpatch_address__class_not_initialized = ((char *)ss) - 1; 
    ss = ret(ss,  Imm_Opnd(4));

    signed offset = (signed)ss - (signed)backpatch_address__class_not_initialized - 1;
    *backpatch_address__class_not_initialized = (char)offset;
    ss = gen_setup_j2n_frame(ss);
    ss = push(ss,  M_Base_Opnd(esp_reg, m2n_sizeof_m2n_frame));
    ss = call(ss, (char *)class_initialize);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(4));
    ss = gen_pop_j2n_frame(ss);
    ss = ret(ss,  Imm_Opnd(4));

    addr = stub;
    assert((ss - stub) <= stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_initialize_class_naked", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_initialize_class_naked", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__vm_initialize_class_naked", ss - stub);
#endif
    return addr;
} //getaddress__vm_initialize_class_naked



//////////////////////////////////////////////////////////////////////
// Object allocation
//////////////////////////////////////////////////////////////////////

static void *getaddress__vm_alloc_java_object_resolved_naked()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    if (VM_Global_State::loader_env->use_lil_stubs) {
        LilCodeStub* cs = lil_parse_code_stub(
            "entry 0:managed:pint:ref;   // The one arg is a ClassHandle for the class of the object we want to create \n"
            "in2out platform:ref; \
             call %0i; \
             jc r=0,fast_alloc_failed; \
             ret; \
             :fast_alloc_failed; \
             push_m2n 0, 0; \
             in2out platform:ref; \
             call %1i; \
             pop_m2n; \
             ret;",
            (void *)class_alloc_new_object_or_null,
            (void *)class_alloc_new_object);
        assert(lil_is_valid(cs));
        addr = LilCodeGenerator::get_platform()->compile(cs, "vm_alloc_java_object_resolved_naked", dump_stubs);
        lil_free_code_stub(cs);
        return addr;
    }

    const int stub_size = 44;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_MAX/2, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;
    ss = push(ss,  M_Base_Opnd(esp_reg, 4));
    ss = call(ss, (char *)class_alloc_new_object_or_null);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(4));
    ss = alu(ss, or_opc,  eax_opnd,  eax_opnd);
    ss = branch8(ss, Condition_Z,  Imm_Opnd(size_8, 0));
    char *backpatch_address__fast_alloc_failed = ((char *)ss) - 1;
    ss = ret(ss,  Imm_Opnd(4));

    signed offset = (signed)ss - (signed)backpatch_address__fast_alloc_failed - 1;
    *backpatch_address__fast_alloc_failed = (char)offset;    
    ss = gen_setup_j2n_frame(ss);
    ss = push(ss,  M_Base_Opnd(esp_reg, m2n_sizeof_m2n_frame));
    ss = call(ss, (char *)class_alloc_new_object);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(4));
    ss = gen_pop_j2n_frame(ss);
    ss = ret(ss,  Imm_Opnd(4));

    addr = stub;
    assert((ss - stub) <= stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_alloc_java_object_resolved_naked", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_alloc_java_object_resolved_naked", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs) {
        dump(stub, "getaddress__vm_alloc_java_object_resolved_naked", ss - stub);
    }
#endif
    return addr;
} //getaddress__vm_alloc_java_object_resolved_naked


static void *generate_object_allocation_stub_with_thread_pointer(char *fast_obj_alloc_proc,
                                                                 char *slow_obj_alloc_proc,
                                                                 char *stub_name)
{
    const int stub_size = 52+26;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_MAX/2, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;

#ifdef PLATFORM_POSIX
    ss = call(ss, (char *)vm_get_gc_thread_local);
#else // !PLATFORM_POSIX
    *ss++ = (char)0x64;
    *ss++ = (char)0xa1;
    *ss++ = (char)0x14;
    *ss++ = (char)0x00;
    *ss++ = (char)0x00;
    *ss++ = (char)0x00;
    ss = alu(ss, add_opc,  eax_opnd,  Imm_Opnd((uint32)&((VM_thread *)0)->_gc_private_information));
#endif // !PLATFORM_POSIX
    ss = push(ss,  eax_opnd);

    ss = push(ss,  M_Base_Opnd(esp_reg, 12));
    ss = push(ss,  M_Base_Opnd(esp_reg, 12));
    ss = call(ss, (char *)fast_obj_alloc_proc);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(12));

    ss = alu(ss, or_opc,  eax_opnd,  eax_opnd);

    ss = branch8(ss, Condition_Z,  Imm_Opnd(size_8, 0));
    char *backpatch_address__fast_alloc_failed = ((char *)ss) - 1;

    ss = ret(ss,  Imm_Opnd(8));

    signed offset = (signed)ss - (signed)backpatch_address__fast_alloc_failed - 1;
    *backpatch_address__fast_alloc_failed = (char)offset;
    
    ss = gen_setup_j2n_frame(ss);
#ifdef PLATFORM_POSIX
    ss = call(ss, (char *)vm_get_gc_thread_local);
#else // !PLATFORM_POSIX
    *ss++ = (char)0x64;
    *ss++ = (char)0xa1;
    *ss++ = (char)0x14;
    *ss++ = (char)0x00;
    *ss++ = (char)0x00;
    *ss++ = (char)0x00;
    ss = alu(ss, add_opc,  eax_opnd,  Imm_Opnd((uint32)&((VM_thread *)0)->_gc_private_information));
#endif // !PLATFORM_POSIX
    ss = push(ss,  eax_opnd);
    ss = push(ss,  M_Base_Opnd(esp_reg, 8+m2n_sizeof_m2n_frame));
    ss = push(ss,  M_Base_Opnd(esp_reg, 8+m2n_sizeof_m2n_frame));
    ss = call(ss, (char *)slow_obj_alloc_proc);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(12));

    ss = gen_pop_j2n_frame(ss);

    ss = ret(ss,  Imm_Opnd(8));

    assert((ss - stub) <= stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("object_allocation_stub_with_thread_pointer", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("object_allocation_stub_with_thread_pointer", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, stub_name, ss - stub);
#endif
    return (void *)stub;
} //generate_object_allocation_stub_with_thread_pointer


static void *getaddress__vm_alloc_java_object_resolved_using_vtable_and_size_naked()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    addr = generate_object_allocation_stub_with_thread_pointer((char *) gc_alloc_fast,
        (char *) vm_malloc_with_thread_pointer,
        "getaddress__vm_alloc_java_object_resolved_using_thread_pointer_naked");
    
    return addr;
} //getaddress__vm_alloc_java_object_resolved_using_vtable_and_size_naked


static void* vm_aastore_nullpointer()
{
    static NativeCodePtr addr = NULL;
    if (addr) return addr;

    const int stub_size = 9;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_COLD, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;    

    Imm_Opnd imm1(12);
    ss = alu(ss, sub_opc,  esp_opnd,  imm1);
    ss = push(ss,  eax_opnd);
    ss = jump(ss, (char*)exn_get_rth_throw_null_pointer());

    addr = stub;
    assert((ss - stub) <= stub_size);
#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "vm_aastore_nullpointer", ss - stub);
#endif
    return addr;
} //vm_aastore_nullpointer


static void* vm_aastore_array_index_out_of_bounds()
{
    static NativeCodePtr addr = NULL;
    if (addr) return addr;

    const int stub_size = 9;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_COLD, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;    

    Imm_Opnd imm1(12);
    ss = alu(ss, sub_opc,  esp_opnd,  imm1);
    ss = push(ss,  eax_opnd);
    ss = jump(ss, (char*)exn_get_rth_throw_array_index_out_of_bounds());

    addr = stub;
    assert((ss - stub) <= stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_aastore_nullpointer", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_aastore_nullpointer", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub,"vm_aastore_array_index_out_of_bounds",  ss - stub);
#endif
    return addr;
} //vm_aastore_array_index_out_of_bounds


static void* vm_aastore_arraystore()
{
    static NativeCodePtr addr = NULL;
    if (addr != NULL) {
        return addr;
    }

    const int stub_size = 9;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_COLD, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;    

    Imm_Opnd imm1(12);
    ss = alu(ss, sub_opc,  esp_opnd,  imm1);
    ss = push(ss,  eax_opnd);
    ss = jump(ss, (char*)exn_get_rth_throw_array_store());

    addr = stub;
    assert((ss - stub) <= stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_aastore_arraystore", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_aastore_arraystore", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "vm_aastore_arraystore", ss - stub);
#endif
    return addr;
} //vm_aastore_arraystore



static void *__stdcall
aastore_ia32(volatile ManagedObject *elem,
             int idx,
             Vector_Handle array) stdcall__;


// 20030321 This JIT support routine expects to be called directly from managed code. 
static void *__stdcall
aastore_ia32(volatile ManagedObject *elem,
            int idx,
            Vector_Handle array)
{
    if (VM_Global_State::loader_env->compress_references) {
        // 20030321 Convert a null reference from a managed (heap_base) to an unmanaged null (NULL/0).
        if (elem == (volatile ManagedObject *)Class::heap_base) {
            elem = NULL;
        }
        if (array == (ManagedObject *)Class::heap_base) {
            array = NULL;
        }
    }

    assert ((elem == NULL) || (((ManagedObject *)elem)->vt() != NULL));
#ifdef VM_STATS
    vm_stats_total.num_aastore++;
#endif // VM_STATS
    void *new_eip = 0;
    if (array == NULL) {
        new_eip = vm_aastore_nullpointer();
    } else if ((unsigned)get_vector_length(array) <= (unsigned)idx) {
        new_eip = vm_aastore_array_index_out_of_bounds();
    } else {
        assert(idx >= 0);
        if (elem != NULL) {
            VTable *vt = get_vector_vtable(array);
#ifdef VM_STATS
            if (vt == cached_object_array_vtable_ptr)
                vm_stats_total.num_aastore_object_array ++;
            if (vt->clss->array_element_class->vtable == ((ManagedObject *)elem)->vt())
                vm_stats_total.num_aastore_equal_type ++;
            if (vt->clss->array_element_class->is_suitable_for_fast_instanceof)
                vm_stats_total.num_aastore_fast_decision ++;
#endif // VM_STATS
            if(vt == cached_object_array_vtable_ptr ||
                class_is_subtype_fast(((ManagedObject *)elem)->vt(), vt->clss->array_element_class)) {
                STORE_REFERENCE((ManagedObject *)array, get_vector_element_address_ref(array, idx), (ManagedObject *)elem);
                return 0;           
            }
            new_eip = vm_aastore_arraystore();
        } else {
            // A null reference. No need to check types for a null reference.
            assert(elem == NULL);
#ifdef VM_STATS
            vm_stats_total.num_aastore_null ++;
#endif // VM_STATS
            // 20030502 Someone earlier commented out a call to the GC interface function gc_heap_slot_write_ref() and replaced it
            // by code to directly store a NULL in the element without notifying the GC. I've retained that change here but I wonder if
            // there could be a problem later with, say, concurrent GCs.
            if (VM_Global_State::loader_env->compress_references) {
                COMPRESSED_REFERENCE *elem_ptr = (COMPRESSED_REFERENCE *)get_vector_element_address_ref(array, idx);
                *elem_ptr = (COMPRESSED_REFERENCE)NULL;
            } else {
                ManagedObject **elem_ptr = get_vector_element_address_ref(array, idx);
                *elem_ptr = (ManagedObject *)NULL;
            }
            return 0;
        }
    }

    // This may possibly break if the C compiler applies very aggresive optimizations.
    void **saved_eip = ((void **)&elem) - 1;
    void *old_eip = *saved_eip;
    *saved_eip = new_eip;
    return old_eip;
} //aastore_ia32


static void *getaddress__vm_aastore()
{
    assert(VM_Global_State::loader_env->use_lil_stubs);
    static void *addr = NULL;
    if (addr != NULL) {
        return addr;
    }

    LilCodeStub* cs = lil_parse_code_stub(
        "entry 0:managed:ref,pint,ref:void;   // The args are the element ref to store, the index, and the array to store into\n"
        "in2out managed:pint; "
        "call %0i;                            // vm_rt_aastore either returns NULL or the ClassHandle of an exception to throw \n"
        "jc r!=0,aastore_failed; \
         ret; \
         :aastore_failed; \
         std_places 1; \
         sp0=r; \
         tailcall %1i;",
        (void *)vm_rt_aastore,
        exn_get_rth_throw_lazy_trampoline());
    assert(lil_is_valid(cs));
    addr = LilCodeGenerator::get_platform()->compile(cs, "vm_aastore", dump_stubs);
    lil_free_code_stub(cs);
    return addr;
} //getaddress__vm_aastore



static void * gen_new_vector_stub(char *stub_name, char *fast_new_vector_proc, char *slow_new_vector_proc)
{
    const int stub_size = 52;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_MAX/2, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;

    ss = push(ss,  M_Base_Opnd(esp_reg, 8));   // repush length
    ss = push(ss,  M_Base_Opnd(esp_reg, 8));   // repush vector_class
    ss = call(ss, (char *)fast_new_vector_proc);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(8));

    ss = alu(ss, or_opc,  eax_opnd,  eax_opnd);
    ss = branch8(ss, Condition_Z,  Imm_Opnd(size_8, 0));
    char *backpatch_address__fast_alloc_failed = ((char *)ss) - 1;

    ss = ret(ss,  Imm_Opnd(8));

    signed offset = (signed)ss - (signed)backpatch_address__fast_alloc_failed - 1;
    *backpatch_address__fast_alloc_failed = (char)offset;

    ss = gen_setup_j2n_frame(ss);
    ss = push(ss,  M_Base_Opnd(esp_reg, m2n_sizeof_m2n_frame+4));
    ss = push(ss,  M_Base_Opnd(esp_reg, m2n_sizeof_m2n_frame+4));
    ss = call(ss, (char *)slow_new_vector_proc);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(8));

    ss = gen_pop_j2n_frame(ss);

    ss = ret(ss,  Imm_Opnd(8));

    assert((ss - stub) <= stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_new_vector_naked", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_new_vector_naked", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, stub_name, ss - stub);
#endif
    return stub;
} //gen_new_vector_stub


static void *getaddress__vm_new_vector_naked()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    addr = gen_new_vector_stub("getaddress__vm_new_vector_naked", 
        (char *)vm_new_vector_or_null, (char *)vm_new_vector);
    return addr;
} //getaddress__vm_new_vector_naked


static void *getaddress__vm_new_vector_using_vtable_naked() {
    static void *addr = 0;
    if (addr) {
        return addr;
    }
    
    addr = generate_object_allocation_stub_with_thread_pointer((char *)vm_new_vector_or_null_using_vtable_and_thread_pointer,
        (char *)vm_new_vector_using_vtable_and_thread_pointer,
        "getaddress__vm_new_vector_using_vtable_naked");
    return addr;
} //getaddress__vm_new_vector_using_vtable_naked


// This is a __cdecl function and the caller must pop the arguments.
static void *getaddress__vm_multianewarray_resolved_naked()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    const int stub_size = 47;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_DEFAULT, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;

    ss = gen_setup_j2n_frame(ss);
    ss = mov(ss,  ecx_opnd,  M_Base_Opnd(esp_reg, m2n_sizeof_m2n_frame+4));

    ss = lea(ss,  eax_opnd,  M_Index_Opnd(esp_reg, ecx_reg, m2n_sizeof_m2n_frame+4, 2));
    
    char *address_push_count_arg = (char *)ss;

    ss = push(ss,  M_Base_Opnd(eax_reg, 0) );
    ss = alu(ss, sub_opc,  eax_opnd,  Imm_Opnd(4));
    ss = dec(ss,  ecx_opnd);

    signed offset = (signed)address_push_count_arg - (signed)ss - 2;
    ss = branch8(ss, Condition_NZ,  Imm_Opnd(size_8, offset));

    ss = push(ss,  M_Base_Opnd(eax_reg, 0) );
    ss = push(ss,  M_Base_Opnd(eax_reg, -4) );

    ss = call(ss, (char *)vm_multianewarray_resolved);

    ss = mov(ss,  ecx_opnd,  M_Base_Opnd(esp_reg, +4) );

    ss = lea(ss,  esp_opnd,  M_Index_Opnd(esp_reg, ecx_reg, +8, 2) );

    ss = gen_pop_j2n_frame(ss);
    ss = ret(ss);

    addr = stub;
    assert((ss - stub) <= stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_multinewarray_resolved_naked", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_multinewarray_resolved_naked", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__vm_multianewarray_resolved_naked", ss - stub);
#endif
    return addr;
} //getaddress__vm_multianewarray_resolved_naked



static void *getaddress__vm_instantiate_cp_string_naked()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    const int stub_size = 52;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_COLD, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;
    ss = gen_setup_j2n_frame(ss);
    ss = push(ss,  M_Base_Opnd(esp_reg, m2n_sizeof_m2n_frame+4));
    ss = push(ss,  M_Base_Opnd(esp_reg, m2n_sizeof_m2n_frame+4));
    ss = call(ss, (char *)vm_instantiate_cp_string_slow);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(8));
    ss = gen_pop_j2n_frame(ss);
    ss = ret(ss,  Imm_Opnd(8) );
    addr = stub;
    assert((ss - stub) <= stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_instantiate_cp_string_naked", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_instantiate_cp_string_naked", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__vm_instantiate_cp_string_naked", ss - stub);
#endif
    return addr;
} //getaddress__vm_instantiate_cp_string_naked



static void vm_throw_java_lang_IncompatibleClassChangeError()
{
    throw_java_exception("java/lang/IncompatibleClassChangeError");
    ABORT("The last called function should not return");
} //vm_throw_java_lang_IncompatibleClassChangeError



// 20030321 This JIT support routine expects to be called directly from managed code. 
void * getaddress__vm_get_interface_vtable_old_naked()  //wjw verify that this works
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    const int stub_size = (VM_Global_State::loader_env->compress_references? 69 : 50);
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_DEFAULT, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;

    ss = (char *)gen_convert_managed_to_unmanaged_null_ia32((Emitter_Handle)ss, /*stack_pointer_offset*/ +4);

    ss = mov(ss,  edx_opnd,  M_Base_Opnd(esp_reg, +8) );
    ss = mov(ss,  ecx_opnd,  M_Base_Opnd(esp_reg, +4) );
    ss = alu(ss, or_opc,  ecx_opnd,  ecx_opnd);

    ss = branch8(ss, Condition_Z,  Imm_Opnd(size_8, 0));
    char *backpatch_address__null_reference = ((char *)ss) - 1;

    ss = push(ss,  M_Base_Opnd(esp_reg, 8));
    ss = push(ss,  M_Base_Opnd(esp_reg, 8));
    ss = call(ss, (char *)vm_get_interface_vtable);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(8));

    ss = alu(ss, or_opc,  eax_opnd,  eax_opnd);

    ss = branch8(ss, Condition_Z,  Imm_Opnd(size_8, 0));
    char *backpatch_address__interface_not_found = ((char *)ss) - 1;

    ss = ret(ss,  Imm_Opnd(8) );

    signed offset = (signed)ss - (signed)backpatch_address__interface_not_found - 1;
    *backpatch_address__interface_not_found = (char)offset;

    ss = gen_setup_j2n_frame(ss);

    ss = call(ss, (char *)vm_throw_java_lang_IncompatibleClassChangeError);

    offset = (signed)ss - (signed)backpatch_address__null_reference - 1;
    *backpatch_address__null_reference = (char)offset;

    ss = alu(ss, xor_opc,  eax_opnd,  eax_opnd);
    ss = ret(ss,  Imm_Opnd(8) );
    
    addr = stub;
    assert((ss - stub) <= stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_get_interface_vtable_old_naked", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_get_interface_vtable_old_naked", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__vm_get_interface_vtable_old_naked", ss - stub);
#endif
    return addr;
} //getaddress__vm_get_interface_vtable_old_naked


static void vm_throw_java_lang_ArithmeticException()
{
    assert(!tmn_is_suspend_enabled());
    throw_java_exception("java/lang/ArithmeticException");
    ABORT("The last called function should not return");
} //vm_throw_java_lang_ArithmeticException


static void* getaddress__setup_java_to_native_frame()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    const int stub_size = 3+m2n_push_m2n_size(false, 1)+1+1;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_MAX/2, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;

    ss = xchg(ss,  M_Base_Opnd(esp_reg, 0),  ebp_opnd, size_32);
    ss = m2n_gen_push_m2n(ss, NULL, FRAME_UNKNOWN, false, 1);
    ss = push(ss,  ebp_opnd);
    ss = ret(ss);
    
    assert((ss - stub) <= stub_size);
    addr = stub;

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("setup_java_to_native_frame", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("setup_java_to_native_frame", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__setup_java_to_native_frame", ss - stub);
#endif
    return addr;
} //getaddress__setup_java_to_native_frame


VMEXPORT char *gen_setup_j2n_frame(char *s)
{
    s = call(s, (char *)getaddress__setup_java_to_native_frame() );
    return s;
} //setup_j2n_frame


static void* getaddress__pop_java_to_native_frame()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    const int stub_size = 1+m2n_pop_m2n_size(false, 0, 0, 2)+1+1;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_MAX/2, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;

    ss = pop(ss,  ecx_opnd);
    ss = m2n_gen_pop_m2n(ss, false, 0, 0, 2);
    ss = push(ss,  ecx_opnd);
    ss = ret(ss);

    assert((ss - stub) <= stub_size);
    addr = stub;

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("pop_java_to_native_frame", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("pop_java_to_native_frame", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__pop_java_to_native_frame", ss - stub);
#endif
    return addr;
} //getaddress__pop_java_to_native_frame


VMEXPORT char *gen_pop_j2n_frame(char *s)
{
    s = call(s, (char *)getaddress__pop_java_to_native_frame() );
    return s;
} //setup_j2n_frame


/////////////////////////////////////////////////////////////////
// end VM_Runtime_Support
/////////////////////////////////////////////////////////////////


static void
vm_throw_linking_exception(Class *clss,
                           unsigned cp_index,
                           unsigned opcode)
{
    TRACE("vm_throw_linking_exception, idx=" << cp_index << "\n");
    class_throw_linking_error(clss, cp_index, opcode);
    ABORT("The last called function should not return");
} //vm_throw_linking_exception


void * getaddress__vm_throw_linking_exception_naked()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    const int stub_size = 100;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_COLD, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;

    ss = gen_setup_j2n_frame(ss);
    ss = push(ss,  M_Base_Opnd(esp_reg, m2n_sizeof_m2n_frame+8));
    ss = push(ss,  M_Base_Opnd(esp_reg, m2n_sizeof_m2n_frame+8));
    ss = push(ss,  M_Base_Opnd(esp_reg, m2n_sizeof_m2n_frame+8));
    ss = call(ss, (char *)vm_throw_linking_exception);
    
    addr = stub;
    assert((ss - stub) < stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_throw_linking_exception_naked", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_throw_linking_exception_naked", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__vm_throw_linking_exception_naked", ss - stub);
#endif
    return addr;
} //getaddress__vm_throw_linking_exception_naked



// 20030321 This JIT support routine expects to be called directly from managed code. 
void * getaddress__gc_write_barrier_fastcall()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    const int stub_size = 11;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_MAX/2, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;

    ss = push(ss,  ecx_opnd);

    if (VM_Global_State::loader_env->compress_references) {
        // 20030321 Convert a null reference in %ecx from a managed (heap_base) to an unmanaged null (0/NULL). 
        ss = test(ss,  ecx_opnd,  Imm_Opnd((unsigned)Class::heap_base));
        ss = branch8(ss, Condition_NE,  Imm_Opnd(size_8, 0));  // branch around mov 0
        char *backpatch_address__not_managed_null = ((char *)ss) - 1;
        ss = mov(ss,  ecx_opnd,  Imm_Opnd(0));
        signed offset = (signed)ss - (signed)backpatch_address__not_managed_null - 1;
        *backpatch_address__not_managed_null = (char)offset;
    }

    ss = call(ss, (char *)gc_write_barrier);
    ss = alu(ss, add_opc,  esp_opnd,  Imm_Opnd(4));

    ss = ret(ss);

    addr = stub;
    assert((ss - stub) < stub_size);

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("gc_write_barrier_fastcall", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("gc_write_barrier_fastcall", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__gc_write_barrier_fastcall", ss - stub);
#endif
    return addr;
} //getaddress__gc_write_barrier_fastcall


static int64 __stdcall vm_lrem(int64 m, int64 n) stdcall__;

static int64 __stdcall vm_lrem(int64 m, int64 n)
{
    assert(!tmn_is_suspend_enabled());
    return m % n;
} //vm_lrem


void * getaddress__vm_lrem_naked()
{
    static void *addr = 0;
    if (addr) {
        return addr;
    }

    const int stub_size = 25;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_DEFAULT, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0xcc /*int 3*/, stub_size);
#endif
    char *ss = stub;
    ss = mov(ss,  eax_opnd,  M_Base_Opnd(esp_reg, +12) );
    ss = alu(ss, or_opc,  eax_opnd,  M_Base_Opnd(esp_reg, +16));
    ss = branch8(ss, Condition_Z,  Imm_Opnd(size_8, 0));
    char *backpatch_address__divide_by_zero = ((char *)ss) - 1;
 
    ss = jump(ss, (char *)vm_lrem);

    signed offset = (signed)ss - (signed)backpatch_address__divide_by_zero - 1;
    *backpatch_address__divide_by_zero = (char)offset;

    ss = gen_setup_j2n_frame(ss);
    ss = call(ss, (char *)vm_throw_java_lang_ArithmeticException);
    assert((ss - stub) <= stub_size);
    addr = stub;

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_lrem_naked", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_lrem_naked", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__vm_lrem_naked", ss - stub);
#endif
    return addr;
} //getaddress__vm_lrem_naked


static int64 __stdcall vm_ldiv(int64 m, int64 n) stdcall__;

static int64 __stdcall vm_ldiv(int64 m, int64 n)
{
    assert(!tmn_is_suspend_enabled());
    assert(n);
    return m / n;
} //vm_ldiv


static void *getaddress__vm_ldiv_naked()
{
    static void *addr = 0;
    if(addr) {
        return addr;
    }

    const int stub_size = 25;
    char *stub = (char *)malloc_fixed_code_for_jit(stub_size, DEFAULT_CODE_ALIGNMENT, CODE_BLOCK_HEAT_DEFAULT, CAA_Allocate);
#ifdef _DEBUG
    memset(stub, 0x90, stub_size);   // nop
#endif
    char *s = stub;
    s = mov(s,  eax_opnd,  M_Base_Opnd(esp_reg, 12));
    s = alu(s, or_opc,  eax_opnd,  M_Base_Opnd(esp_reg, 16));
    s = branch8(s, Condition_E,  Imm_Opnd(size_8, 5));  // skip 5 bytes over the next instruction
    s = jump32(s,  Imm_Opnd((((uint32)vm_ldiv) - ((uint32)s)) - 5));

    s = gen_setup_j2n_frame(s);

    s = call(s, (char *)vm_throw_java_lang_ArithmeticException);


    assert((s - stub) <= stub_size);
    addr = stub;

    if (VM_Global_State::loader_env->TI->isEnabled())
    {
        jvmti_add_dynamic_generated_code_chunk("vm_ldiv_naked", stub, stub_size);
        jvmti_send_dynamic_code_generated_event("vm_ldiv_naked", stub, stub_size);
    }

#ifndef NDEBUG
    if (dump_stubs)
        dump(stub, "getaddress__vm_ldiv_naked", s - stub);
#endif
    return addr;
} //getaddress__vm_ldiv_naked






#ifdef VM_STATS

static void register_request_for_rt_function(VM_RT_SUPPORT f) {
    // Increment the number of times that f was requested by a JIT. This is not the number of calls to that function,
    // but this does tell us how often a call to that function is compiled into JITted code.
    vm_stats_total.rt_function_requests.add((void *)f, /*value*/ 1, /*value1*/ NULL);
} //register_request_for_rt_function

#endif //VM_STATS


void *vm_get_rt_support_addr(VM_RT_SUPPORT f)
{
#ifdef VM_STATS
    register_request_for_rt_function(f);
#endif // VM_STATS

        NativeCodePtr res = rth_get_lil_helper(f);
        if (res) return res;

    switch(f) {
    case VM_RT_NULL_PTR_EXCEPTION:
        return exn_get_rth_throw_null_pointer();
    case VM_RT_IDX_OUT_OF_BOUNDS:
        return exn_get_rth_throw_array_index_out_of_bounds();
    case VM_RT_ARRAY_STORE_EXCEPTION:
        return exn_get_rth_throw_array_store();
    case VM_RT_DIVIDE_BY_ZERO_EXCEPTION:
        return exn_get_rth_throw_arithmetic();
    case VM_RT_THROW:
    case VM_RT_THROW_SET_STACK_TRACE:
        return exn_get_rth_throw();
    case VM_RT_THROW_LAZY:
        return exn_get_rth_throw_lazy();
    case VM_RT_LDC_STRING:
        return getaddress__vm_instantiate_cp_string_naked();
    case VM_RT_NEW_RESOLVED:
        return getaddress__vm_alloc_java_object_resolved_naked();
    case VM_RT_NEW_RESOLVED_USING_VTABLE_AND_SIZE:
        return getaddress__vm_alloc_java_object_resolved_using_vtable_and_size_naked(); 
    case VM_RT_MULTIANEWARRAY_RESOLVED:
        return getaddress__vm_multianewarray_resolved_naked();
    case VM_RT_NEW_VECTOR:
        return getaddress__vm_new_vector_naked();
    case VM_RT_NEW_VECTOR_USING_VTABLE:
        return getaddress__vm_new_vector_using_vtable_naked();
    case VM_RT_AASTORE:
        if (VM_Global_State::loader_env->use_lil_stubs) {
            return getaddress__vm_aastore();
        } else {
            return (void *)aastore_ia32;
        }
    case VM_RT_AASTORE_TEST:
        return (void *)vm_aastore_test;
    case VM_RT_WRITE_BARRIER_FASTCALL:
        return getaddress__gc_write_barrier_fastcall();

    case VM_RT_CHECKCAST:
        return getaddress__vm_checkcast_naked();

    case VM_RT_INSTANCEOF:
    return getaddress__vm_instanceof();

    case VM_RT_MONITOR_ENTER:
    case VM_RT_MONITOR_ENTER_NO_EXC:
        return getaddress__vm_monitor_enter_naked();

    case VM_RT_MONITOR_ENTER_STATIC:
        return getaddress__vm_monitor_enter_static_naked();

    case VM_RT_MONITOR_EXIT:
    case VM_RT_MONITOR_EXIT_NON_NULL:
        return getaddress__vm_monitor_exit_naked();

    case VM_RT_MONITOR_EXIT_STATIC:
        return getaddress__vm_monitor_exit_static_naked();

    case VM_RT_GET_INTERFACE_VTABLE_VER0:
        return getaddress__vm_get_interface_vtable_old_naked();  //tryitx
    case VM_RT_INITIALIZE_CLASS:
        return getaddress__vm_initialize_class_naked();
    case VM_RT_THROW_LINKING_EXCEPTION:
        return getaddress__vm_throw_linking_exception_naked();

    case VM_RT_LREM:
        return getaddress__vm_lrem_naked();
    case VM_RT_LDIV:
        return getaddress__vm_ldiv_naked();

    case VM_RT_F2I:
    case VM_RT_F2L:
    case VM_RT_D2I:
    case VM_RT_D2L:
    case VM_RT_LSHL:
    case VM_RT_LSHR:
    case VM_RT_LUSHR:
    case VM_RT_FREM:
    case VM_RT_DREM:
    case VM_RT_LMUL:
#ifdef VM_LONG_OPT
    case VM_RT_LMUL_CONST_MULTIPLIER:
#endif
    case VM_RT_CONST_LDIV:
    case VM_RT_CONST_LREM:
    case VM_RT_DDIV:
    case VM_RT_IMUL:
    case VM_RT_IDIV:
    case VM_RT_IREM:
    case VM_RT_CHAR_ARRAYCOPY_NO_EXC:
        return get_generic_rt_support_addr_ia32(f);

    default:
        ABORT("Unexpected helper id");
        return 0;
    }
} //vm_get_rt_support_addr



/**************************************************
 * The following code has to do with the LIL stub inlining project.
 * Modifying it should not affect anything.
 */


// a structure used in memoizing already created stubs
struct TypecheckStubMemoizer {
    Class *clss;  // the class for which this stub is for
    void *fast_checkcast_stub, *fast_instanceof_stub;
    TypecheckStubMemoizer *next;

    static TypecheckStubMemoizer* head;  // head of the list
    static  tl::MemoryPool mem;  // we'll alocate structures from here

    static void* find_stub(Class *c, bool is_checkcast) {
        // search for an existing struct for this class
        for (TypecheckStubMemoizer *t = head;  t != NULL;  t = t->next) {
            if (t->clss == c) {
                return (is_checkcast) ?
                    t->fast_checkcast_stub : t->fast_instanceof_stub;
            }
        }

        return NULL;
    }

    static void add_stub(Class *c, void *stub, bool is_checkcast) {
        // search for an existing struct for this class
      
      TypecheckStubMemoizer *t;
      for (t = head;  t != NULL;  t = t->next) {
            if (t->clss == c)
                break;
        }

        if (t == NULL) {
            // create new structure
            t = (TypecheckStubMemoizer*) mem.alloc(sizeof(TypecheckStubMemoizer));
            t->clss = c;
            t->fast_checkcast_stub = NULL;
            t->fast_instanceof_stub = NULL;
            t->next = head;
            head = t;
        }

        if (is_checkcast) {
            assert(t->fast_checkcast_stub == NULL);
            t->fast_checkcast_stub = stub;
        }
        else {
            assert(t->fast_instanceof_stub == NULL);
            t->fast_instanceof_stub = stub;
        }
    }
};

static StaticInitializer jit_runtime_initializer;
TypecheckStubMemoizer* TypecheckStubMemoizer::head = NULL;
tl::MemoryPool TypecheckStubMemoizer::mem;  // we'll alocate structures from here

/* ? 03/07/30: temporary interface change!!! */
void *vm_get_rt_support_addr_optimized(VM_RT_SUPPORT f, Class_Handle c) {
    Class *clss = (Class*) c;
    if (clss == NULL)
    {
        return vm_get_rt_support_addr(f);
    }

    switch (f) {
    case VM_RT_CHECKCAST:
            return vm_get_rt_support_addr(f);
        // break; // remark #111: statement is unreachable
    case VM_RT_INSTANCEOF:
            return vm_get_rt_support_addr(f);
        // break;// remark #111: statement is unreachable
    case VM_RT_NEW_RESOLVED:
        if (class_is_finalizable(c))
            return getaddress__vm_alloc_java_object_resolved_naked();
        else
            return vm_get_rt_support_addr(f);
        // break;// remark #111: statement is unreachable
    case VM_RT_NEW_RESOLVED_USING_VTABLE_AND_SIZE:
        if (class_is_finalizable(c))
            return getaddress__vm_alloc_java_object_resolved_using_vtable_and_size_naked();
        else
            return vm_get_rt_support_addr(f);
        // break;// remark #111: statement is unreachable
    default:
        return vm_get_rt_support_addr(f);
        // break;// remark #111: statement is unreachable
    }
}


// instead of returning a stub address, this support function returns
// parsed LIL code.
// May return NULL if the stub requested should not be inlined
VMEXPORT LilCodeStub *vm_get_rt_support_stub(VM_RT_SUPPORT f, Class_Handle c) {
    Class *clss = (Class *) c;

    switch (f) {
    case VM_RT_CHECKCAST:
    {
        if (!clss->is_suitable_for_fast_instanceof)
            return NULL;

        return gen_lil_typecheck_stub_specialized(true, true, clss);
    }
    case VM_RT_INSTANCEOF:
    {
        if (!clss->is_suitable_for_fast_instanceof)
            return NULL;

        return gen_lil_typecheck_stub_specialized(false, true, clss);
    }
    default:
        ABORT("Unexpected helper");
        return NULL;
    }
}


/*
 * LIL inlining code - end
 **************************************************/
