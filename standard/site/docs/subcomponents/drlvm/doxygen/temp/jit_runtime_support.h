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

#ifndef _JIT_RUNTIME_SUPPORT_H_
#define _JIT_RUNTIME_SUPPORT_H_

#include "open/types.h"
#include "jni_types.h"

/**
 * This is a complete set of functions used by the code generated by the JIT.
 * The address of a function is obtained from the VM by invoking the
 * function <code>vm_get_rt_support_addr</code> (see below).
 *
 * We should also note which exceptions can be thrown by each
 * of those function.
 *
 * If changes are made to enum <code>VM_RT_SUPPORT</code> below, the list of 
 * descriptions in <code>tr_helper_info.cpp</code> must also be changed.
 */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef
enum VM_RT_SUPPORT {

/** Void id marker */
    VM_RT_UNKNOWN=0,

/**
 * Object creation routines.
 */

    VM_RT_NEW_RESOLVED_USING_VTABLE_AND_SIZE=100,
/**
 * @param The parameters are the following:
 *        \arg Size of the instance to be created
 *        \arg <code>Allocation_Handle</code> for the class whose instance we want to create
 * 
 * @return Reference to the new object.
 *
 * Like <code>VM_RT_NEW_VECTOR_USING_VTABLE</code>, allocates a new object of 
 * a class, but also takes a size argument. The size can be obtained using 
 * class_get_boxed_data_size(Class_Handle).
 * This function should be used for classes which inherit the finalize method 
 * of <code>java.lang.Object</code>.
 */

    VM_RT_NEW_VECTOR_USING_VTABLE=101,
/**   
 * @param The parameters are the following:
 *        \arg Vector length
 *        \arg <code>Allocation_Handle</code> for the vector class
 *
 * @return Reference to the new object.
 *
 * Like <code>VM_RT_NEW_VECTOR</code> creates a vector 
 * (zero-based, one dimensional array) of the given type,
 * but takes a <code>Allocation_Handle</code> instead of a 
 * <code>Class_Handle</code>.
 */
    VM_RT_MULTIANEWARRAY_RESOLVED=102,
 /**
 * There is a variable # of arguments:
 * \arg Class handle
 * \arg Number of dimensions
 * \arg Count_n
 * \arg ...
 * \arg Count_1
 *
 * @return Reference to the new object.
 *
 * This is <code>__cdecl</code> function and the caller must pop the arguments.
 * Create a multidimensional Java array.
 */

    VM_RT_LDC_STRING=103,
/**
 * @param The parameters are the following:
 *        \arg Class handle of the class owning the const pool
 *        \arg Const pool index pointing to a CONSTANT_Class.
 *
 * @return \arg Reference to the String object.
 *         \arg Reference to the const string represented by an entry in the
 *         const pool.
 */

/////
// Exception throwing routines
/////

    VM_RT_THROW=200,
/**
 * @param Object reference
 *
 * @return None.
 *
 * The only argument is a reference to the exception to be thrown.
 * Throw the exception without modifying the stack trace associated
 * with the exception object. This function is appropriate for the
 * JVML athrow instruction.
 * This function never returns.
 */

    VM_RT_THROW_SET_STACK_TRACE=210,
 
/** 
 * @param Object reference.
 *
 * @return None.
 *
 * The only argument is a reference to the exception to be thrown.
 * Throw the exception and set the stack trace associated
 * with the exception object. This function is appropriate for the
 * CIL throw instruction. 
 *
 *This function never returns.
 */

    VM_RT_THROW_LAZY=201,
/**
 * @param The parameters are the following:
 *        \arg Method handle
 *        \arg ...
 *        \arg arg 2
 *        \arg arg 1
 *        \arg Class handle
 *
 * @return None.
 *
 * The arguments to the constructor are pushed the same way they would have
 * been pushed to invoke the constructor, except that the (non-existing)
 * this pointer is replaced with the class handle of the exception.
 * The method handle representing the constructor is pushed last.
 *
 * This function never returns.
 */

    VM_RT_IDX_OUT_OF_BOUNDS=202,
 
/**
 * @param none
 *  
 * @return None.
 *
 * Throw the <code>java/lang/ArrayIndexOutOfBoundsException</code>.
 *
 * This function never returns.
 */
    VM_RT_NULL_PTR_EXCEPTION=203,
 
/**
 * @param none
 *
 * @return None.
 *
 * Throw the <code>java/lang/NullPointerException</code>
 *
 * This function never returns.
 */
    VM_RT_DIVIDE_BY_ZERO_EXCEPTION=204,
  
/**
 * @param none
 *
 * @return None.
 *
 * Throw the <code>java/lang/ArithmeticException</code>.
 *
 * This function never returns.
 */

    VM_RT_ARRAY_STORE_EXCEPTION=205,
   
/**
 * @param none
 * 
 * @return None.
 *
 * Throw the <code>java/lang/ArrayStoreException</code>.
 *
 * This function never returns.
 */

    VM_RT_THROW_LINKING_EXCEPTION=206,
/**
 * @param The parameters are the following:
 *        \arg Const pool index
 *        \arg Class handle of the class owning the const pool
 *        \arg Loader exception returned by the VM
 *
 * @return None.
 *
 * Throws a linking exception reported by the VM at compile-time.
 */

 ////
 // Synchronization routines
 //// 

    VM_RT_MONITOR_ENTER=300,
/**
 * @param Object
 *
 * @return None.
 *
 * Acquire the monitor associated with the object.
 * Throw <code>java/lang/NullPointerException</code>, if the argument is null.
 */
    VM_RT_MONITOR_ENTER_NON_NULL=304,
 /**
  * @param Object
  *
  * @return None.
  *
  * Acquire the monitor associated with the object.
  * This function is identical to <code>VM_RT_MONITOR_ENTER</code> except that it
  * assumes that the argument is non-null. Passing a null argument
  * will result in undefined behavior.
  */
    VM_RT_MONITOR_EXIT=301,
/**   
 * @param Object
 *
 * @return None.
 *
 * Release the monitor associated with the object.
 * Throw <code>java/lang/NullPointerException</code>, if the argument is null.
 * Throw <code>java/lang/IllegalMonitorStateException</code>, if the current 
 * thread is not the owner of the lock.
 */
    VM_RT_MONITOR_EXIT_NON_NULL=305,
  
/**
 * @param Object
 *
 * @return None.
 *
 * Release the monitor associated with the object.
 * This function is identical to <code>VM_RT_MONITOR_EXIT</code> except that it
 * assumes that the argument is non-null. Passing a null argument
 * will result in undefined behavior.
 * Throw <code>java/lang/IllegalMonitorStateException</code>, if the current thread
 * is not the owner of the lock.
 */
    VM_RT_MONITOR_ENTER_STATIC=302,
/**  
 * @param Class handle
 *
 * @return None.
 *
 * Acquire the monitor associated with the class passed as the argument.
 */
    VM_RT_MONITOR_EXIT_STATIC=303,
/**
 * @param Class handle
 *
 * @return None.
 *
 * Release the monitor associated with the class passed as the argument.
 * Throw <code>java/lang/IllegalMonitorStateException</code>, if the current thread
 * is not the owner of the lock.
 */

////
// Type access routines
////

    VM_RT_CHECKCAST=400,
/**
 * @param The parameters are the following:
 *        \arg Object
 *        \arg Class
 * 
 * @return Object.
 *
 * If object can't be cast to class, throw <code>ClassCastException</code>
 */

    VM_RT_INSTANCEOF=401,
/**
 * @param The parameters are the following:
 *        \arg Object
 *        \arg Class
 *
 * @return 1 if object is not null and can be cast to class;
 *         0 otherwise
 */
    VM_RT_AASTORE=402,
/**
 * @param The parameters are the following:
 *        \arg Array
 *        \arg Index
 *        \arg Element
 *
 * @return None.
 *
 * Store a reference element into into an array.
 */
    VM_RT_AASTORE_TEST=403,
/**
 * @param The parameters are the following:
 *        \arg Element
 *        \arg Array
 *
 * @return 1 if the element can be stored in the array; 0 otherwise.
 *
 * Check if a reference element into into an array. If the <code>array</code>
 * argument is null, return 0. This function does not throw exceptions.
 */
    VM_RT_GET_INTERFACE_VTABLE_VER0=404,
/**
 * @param The parameters are the following:
 *        \arg Object reference
 *        \arg Class handle for the interface
 *
 * @return Interface vtable with method entries for the implementation
 *         of that interface by the actual class of the object.
 */

 /////
 //Class initialization
 /////


    VM_RT_INITIALIZE_CLASS=500,
/**
 * @param Class handle
 *
 * @return None.
 *
 * If the class hasn't been initialized yet, initialize it.
 *
 * A call to this function must be generated before every putstatic
 * and getstatic unless the JIT can prove that the class would have
 * been already initialized at that point.
 */

 ////
 // Write barrier routines
 ////

    VM_RT_GC_HEAP_WRITE_REF=600,
/**
 * @param The parameters are the following:
 *        \arg Address of the base of the object (or array) being written to
 *        \arg Address of a memory location being written to
 *        \arg Value being written
 *
 * @return None.
 *
 * Write barrier for GC. Updates the slot with the value provided.
 * The type being written is indicated in the name.
 */
    VM_RT_GC_SAFE_POINT=601,
/**
 * @param none
 *
 * @return None.
 *
 * Use this helper to notify GC about safe point in managed code.
 */
    VM_RT_GC_GET_TLS_BASE=602,
/**
 * @param none
 *
 * @return Pointer to int-sized suspension request flag.
 *
 * When the flag is non zero, managed code should call <code>VM_RT_GC_SAFE_POINT</code>
 * helper.
 *
 * @note The flag must be used for read only!
 */
 
 
 /////
 // JVMTI specific routines
 /////  

    VM_RT_JVMTI_METHOD_ENTER_CALLBACK = 700,
/**
 * @param handle of the method which gets control
 *
 * @return None.
 *
 * This call-back should be used to notify about method enter event.
 * Do a call-back when such capability is requested only.
 */
    VM_RT_JVMTI_METHOD_EXIT_CALLBACK = 701,
/**
 * @param The parameters are the following:
 *        \arg handle of the method which is about to lose control
 *        \arg method's return value
 *
 * @return None.
 *
 * This call-back should be used to notify about method exit event.
 * Do a call-back when such capability is requested only.
 */
 
 VM_RT_JVMTI_FIELD_ACCESS_CALLBACK = 702,

/**
 * @param The parameters are the following:
 *        arg\ handle of the field under access
 *        arg\ handle of the method, which accesses field
 *        arg\ location of code which accesses field
 *        arg\ pointer to the reference of the object, which field is beeng
 *             accessed or NULL for static field
 *
 * @return None.
 *
 * Notifies about field access.
 */
    VM_RT_JVMTI_FIELD_MODIFICATION_CALLBACK = 703,
/**
 * @param The parameters are the following:
 *        arg\ handle of the field under modification
 *        arg\ handle of the method, which modifies field
 *        arg\ location of code which modifies field
 *        arg\ pointer to the reference of the object, which field is beeng
 *             modified or NULL for static field
 *        arg\ pointer to the new value for the field
 *
 * @return none
 *
 * Notifies about field modification.
 */

 /////
 /// Runtime resolution routines
 /////

 
    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *
    * @return new object
    *
    * Creates and returns new object for the given (class, cp_index)
    * Loads and initialize class if needed
    */
    VM_RT_NEWOBJ_WITHRESOLVE,

    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *        arg\ Array size
    *
    * @return new array
    *
    * Creates and returns new array of the given size
    * with type referenced by (class, cp_index)
    * Loads and initialize array class if needed
    */
    VM_RT_NEWARRAY_WITHRESOLVE,

    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *
    * @return field offset
    *
    * Returns an offset of the field referenced
    * by the given (class, cp_index) pair
    * Field's class must be loaded and  initialized
    * before this helper call.
    */
    VM_RT_GET_NONSTATIC_FIELD_OFFSET_WITHRESOLVE,

    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *
    * @return field address
    *
    * Returns an address of the field referenced
    * by the given (class, cp_index) pair
    * Loads and initializes field's class if needed
    */
    VM_RT_GET_STATIC_FIELD_ADDR_WITHRESOLVE,

    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *        arg\ Object to check cast
    *
    * @return third parameter
    *
    * Check if the given object can be casted to 
    * the type referenced by (class, cp_index) pair
    * Throws class cast exception if object can't be casted
    * Returns the object instance (3rd parameter) if cast is allowed
    * Loads and intialize cast type if needed.
    */
    VM_RT_CHECKCAST_WITHRESOLVE,
    
    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *        arg\ Object to check cast
    *
    * @return TRUE or FALSE
    *
    * Check if the given object is instance of 
    * the type referenced by (class, cp_index) pair
    * Return TRUE if object is instance of the given type
    * or FALSE otherwise
    * Loads and intialize 'instanceof' type if needed.
    */
    VM_RT_INSTANCEOF_WITHRESOLVE,

    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *
    * @return indirect address of the static method
    *
    * Returns the indirect address of the static method referenced
    * by (class, cp_index) pair
    * Loads and intialize method's class type if needed.
    */
    VM_RT_GET_INVOKESTATIC_ADDR_WITHRESOLVE,

    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *        arg\ Object
    *
    * @return indirect address of the interface method
    *
    * Returns the indirect address of the interface method referenced
    * by (class, cp_index) pair for the given object
    */
    VM_RT_GET_INVOKEINTERFACE_ADDR_WITHRESOLVE,

    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *        arg\ Object
    *
    * @return indirect address of the virtual method
    *
    * Returns the indirect address of the virtual method referenced
    * by (class, cp_index) pair for the given object
    */
    VM_RT_GET_INVOKEVIRTUAL_ADDR_WITHRESOLVE,
    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *
    * @return indirect address of the special method
    *
    * Returns the indirect address of the special method referenced
    * by (class, cp_index) pair 
    */
    VM_RT_GET_INVOKE_SPECIAL_ADDR_WITHRESOLVE,

    /**
    * @param The parameters are the following:
    *        arg\ Class_Handle - enclosing class handle
    *        arg\ Constant pool index
    *
    * @return Class_Handle
    *
    * Returns the class handle referenced
    * by (class, cp_index) pair 
    * Loads and initialize class if needed
    */
    VM_RT_INITIALIZE_CLASS_WITHRESOLVE,


 /////
 // Non-VM specific helpers for the JIT
 ///// 

    VM_RT_F2I=900,
    VM_RT_F2L=901,
    VM_RT_D2I=902,
    VM_RT_D2L=903,
    // Conversions


    VM_RT_LSHL=910,
    VM_RT_LSHR=911,
    VM_RT_LUSHR=912,
    /**
     * @param The parameters are the following:
     * (new version)
     *  arg\ Long value to be shifted
     *  arg\ Int value with count
     * (old version)
     *  arg\ (edx:eax) A long value to be shifted
     *  arg\ (ecx)     Count 
     * @return (edx:eax) The shifted value
     */


    VM_RT_LMUL=920,
#ifdef VM_LONG_OPT
    VM_RT_LMUL_CONST_MULTIPLIER=921,
#endif
    VM_RT_LREM=922,
    VM_RT_LDIV=923,
    VM_RT_ULDIV=924,
    VM_RT_CONST_LDIV=925,
    VM_RT_CONST_LREM=926,
    // Long arithmetic

    VM_RT_IMUL=930,
    VM_RT_IREM=931,
    VM_RT_IDIV=932,
    // Int arithmetic

    VM_RT_FREM=940,
    VM_RT_FDIV=941,
    VM_RT_DREM=942,
    VM_RT_DDIV=943,
    // Float/double arithmetic

    VM_RT_CHAR_ARRAYCOPY_NO_EXC=950,
/**
 * @param The parameters are the following:
 *        arg\ Object reference for the source array. Must be non-null and refer to an array 
 *             of 16 bit chars
 *        arg\ int32 containing the starting index of the source array to copy
 *        arg\ Object reference for the destination array. Must be non-null and refer to an array 
 *             of 16 bit chars.
 *        arg\ int32 containing the starting index of the destination array to copy into.
 *        arg\ int32 containing the number of 16 bit chars to copy.
 *
 * @return None.
 * 
 * This runtime helper function provides a specialized implementation for <code>System.arraycopy</code>
 * for the common case where the elements of the arrays are 
 * 16 bit characters and the array references are both non-null. The JIT must guarantee
 * that no exceptions will need to be thrown. That is, both array references are non-null,
 * the types match (and are both arrays of 16 bit characters), 
 * and the starting indexes and length are such that the copying will only access
 * data in the bounds of the arrays.
 */

 ////
 // Deprecated routines
 ////

    VM_RT_NEW_RESOLVED=1000,
/**
 * @param Class handle for the class whose object we want to create
 *
 * @return Reference to the new object.
 *
 * Allocates a new object of the class. This function should be used
 * for classes which inherit the finalize method of <code>java.lang.Object</code>.
 * See also <code>VM_RT_NEW_RESOLVED_USING_VTABLE</code>.
 */
    VM_RT_NEW_VECTOR=1001,
/**
 * @param The parameters are the following:
 *        arg\ Vector length
 *        arg\ Class handle of the vector class
 *
 * @return Reference to the new object
 *
 * Create a vector (zero-based, one dimensional array)
 * of the given type. See also <code>VM_RT_NEW_VECTOR_USING_VTABLE</code>.
 */
    VM_RT_WRITE_BARRIER_FASTCALL=1002
/**
 * @param The parameters are the following:
 *        arg\ Address of a memory location being written to
 *        arg\ Object reference being written.
 *
 * @return None.
 *
 * Write barrier for GC.
 */

} VM_RT_SUPPORT;

/** 
* VM RT helpers have different calling conventions.
*/
enum HELPER_CALLING_CONVENTION {
    CALLING_CONVENTION_DRL = 0,
    CALLING_CONVENTION_STDCALL,
    CALLING_CONVENTION_CDECL,
};

/** 
 * VM RT helpers can be interrupted differently.
 */
typedef
enum HELPER_INTERRUPTIBILITY_KIND {
/** 
 * Disallowed to interrupt a thread inside the helper. 
 */
    INTERRUPTIBLE_NEVER,
/** 
 * Some helpers can be run in fast path mode, 
 * when they are not interruptible, or in slow path, when they are.
 */
    INTERRUPTIBLE_SOMETIMES,
/**
 * Helper call can always be interrupted.
 */
    INTERRUPTIBLE_ALWAYS

} HELPER_INTERRUPTIBILITY_KIND;

NativeCodePtr rth_get_lil_helper(VM_RT_SUPPORT f);

VMEXPORT void *vm_get_rt_support_addr(VM_RT_SUPPORT f);


/**
 * Temporary interface addition
 * same as <code>vm_get_rt_support_addr</code>, but tries to optimize the stubs it creates,
 * by specializing them.
 */
VMEXPORT void *vm_get_rt_support_addr_optimized(VM_RT_SUPPORT f, Class_Handle c);

/**
 * Instead of returning a stub address, this support function returns
 * parsed LIL code.
 */
 typedef struct LilCodeStub LilCodeStub;
VMEXPORT LilCodeStub *vm_get_rt_support_stub(VM_RT_SUPPORT f, Class_Handle c);


/**
 *  Checks if helper is a suspension point
 */
VMEXPORT HELPER_INTERRUPTIBILITY_KIND vm_helper_get_interruptibility_kind(VM_RT_SUPPORT f);

VMEXPORT HELPER_CALLING_CONVENTION vm_helper_get_calling_convention(VM_RT_SUPPORT f);
VMEXPORT const char* vm_helper_get_name(VM_RT_SUPPORT id);

/**
 * Returns number of helper arguments.
 * Intended primarily for debugging.
 */
VMEXPORT int vm_helper_get_numargs(VM_RT_SUPPORT id);

/**
* Returns Id of runtime helper by its string representation. 
* Name comparison is case-insensitive.
* If the helperName is unknown, then VM_RT_UNKNOWN is returned.
*/
VMEXPORT VM_RT_SUPPORT vm_helper_get_by_name(const char* name);

#ifndef Global_Env
struct Global_Env;
#endif

jint helper_magic_init(Global_Env * vm_env);

VMEXPORT jint vm_helper_register_magic_helper(VM_RT_SUPPORT id, 
                                   const char* class_name, 
                                   const char* method_name);

VMEXPORT Method_Handle vm_helper_get_magic_helper(VM_RT_SUPPORT id);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !_JIT_RUNTIME_SUPPORT_H_