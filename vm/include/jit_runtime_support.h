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
/** 
 * @author Intel, Evgueni Brevnov, Ivan Volosyuk
 * @version $Revision: 1.1.2.2.4.4 $
 */  




#ifndef _JIT_RUNTIME_SUPPORT_H_
#define _JIT_RUNTIME_SUPPORT_H_

#include "open/types.h"

//
// This is a complete set of functions used by the code generated by the JIT.
// The address of a function is obtained from the VM by invoking the
// function vm_get_rt_support_addr (see below).
//

// We should also note which exceptions can be thrown by each
// of those function.

// If changes are made to enum VM_RT_SUPPORT below, the list of JIT support functions in vm_stats.cpp must also be changed.


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef
enum VM_RT_SUPPORT {

    //////////////////////////////////////////////////////////////////////////
    // Object creation routines
    //////////////////////////////////////////////////////////////////////////

    VM_RT_NEW_RESOLVED_USING_VTABLE_AND_SIZE=100,
    // Arguments:
    //  - Size of the instance to be created
    //  - Allocation_Handle for the class whose instance we want to create
    // Return value:
    //  - Reference to the new object
    //
    // Like VM_RT_NEW_VECTOR_USING_VTABLE, allocates a new object of a class, but also takes a
    // size argument.  The size can be obtained using class_get_boxed_data_size(Class_Handle).
    // This function should be used for classes which inherit the finalize method 
    // of java.lang.Object.

    VM_RT_NEW_VECTOR_USING_VTABLE=101,
    // Arguments:
    //  - Allocation_Handle for the vector class
    //  - Vector length
    // Return value:
    //  - Reference to the new object
    //
    // Like VM_RT_NEW_VECTOR. creates a vector (zero-based, one dimensional array) of the given type,
    // but takes a Allocation_Handle instead of a Class_Handle.

    VM_RT_MULTIANEWARRAY_RESOLVED=102,
    // There is a variable # of arguments:
    //  - Class handle
    //  - Number of dimensions
    //  - Count_n
    //  - ...
    //  - Count_1
    // Return value:
    //  - Reference to the new object
    //
    // This is __cdecl function and the caller must pop the arguments.
    // Create a multidimensional Java array
    //

    VM_RT_LDC_STRING=103,
    // Arguments:
    //  - Const pool index pointing to a CONSTANT_Class.
    //  - Class handle of the class owning the const pool
    // Return value:
    //  - Reference to the String object.
    //
    // Returns a reference to the const string represented by an entry in the
    // const pool.



    //////////////////////////////////////////////////////////////////////////
    // Exception throwing routines
    //////////////////////////////////////////////////////////////////////////

    VM_RT_THROW=200,
    // Arguments:
    //  - Object reference
    // Return value: none.
    //
    // The only argument is a reference to the exception to be thrown.
    // Throw the exception without modifying the stack trace associated
    // with the exception object.  This function is appropriate for the
    // JVML athrow instruction.
    // This function never returns.

    VM_RT_THROW_SET_STACK_TRACE=210,
    // Arguments:
    //  - Object reference
    // Return value: none.
    //
    // The only argument is a reference to the exception to be thrown.
    // Throw the exception and set the stack trace associated
    // with the exception object.  This function is appropriate for the
    // CIL throw instruction.
    // This function never returns.


    VM_RT_THROW_LAZY=201,
    // Arguments:
    //  - Method handle
    //  - ...
    //  - arg 2
    //  - arg 1
    //  - Class handle
    // Return value: none.
    //
    // The arguments to the constructor are pushed the same way they would have
    // been pushed to invoke the constructor, except that the (non-existing)
    // this pointer is replaced with the class handle of the exception.
    // The method handle representing the constructor is pushed last.
    //
    // This function never returns.

    VM_RT_IDX_OUT_OF_BOUNDS=202,
    // Arguments: none
    // Return value: none.
    //
    // Throw the java/lang/ArrayIndexOutOfBoundsException
    // This function never returns.

    VM_RT_NULL_PTR_EXCEPTION=203,
    // Arguments: none
    // Return value: none.
    //
    // Throw the java/lang/NullPointerException
    // This function never returns.

    VM_RT_DIVIDE_BY_ZERO_EXCEPTION=204,
    // Arguments: none
    // Return value: none.
    //
    // Throw the java/lang/ArithmeticException
    // This function never returns.

    VM_RT_ARRAY_STORE_EXCEPTION=205,
    // Arguments: none
    // Return value: none.
    //
    // Throw the java/lang/ArrayStoreException
    // This function never returns.

    VM_RT_THROW_LINKING_EXCEPTION=206,
    // Arguments:
    //  - Const pool index.
    //  - Class handle of the class owning the const pool.
    //  - Loader exception returned by the VM.
    // Return value: none.
    //
    // Throws a linking exception reported by the VM at compile-time.


    //////////////////////////////////////////////////////////////////////////
    // Synchronization routines
    //////////////////////////////////////////////////////////////////////////

    VM_RT_MONITOR_ENTER=300,
    // Arguments:
    //  - Object
    // Return value: none.
    //
    // Acquire the monitor associated with the object.
    // Throw java/lang/NullPointerException if the argument is null.

    VM_RT_MONITOR_ENTER_NON_NULL=304,
    // Arguments:
    //  - Object
    // Return value: none.
    //
    // Acquire the monitor associated with the object.
    // This function is identical to VM_RT_MONITOR_ENTER except that it
    // assumes that the argument is non-null.  Passing a null argument
    // will result in undefined behavior.

    VM_RT_MONITOR_EXIT=301,
    // Arguments:
    //  - Object
    // Return value: none.
    //
    // Release the monitor associated with the object.
    // Throw java/lang/NullPointerException if the argument is null.
    // Throw java/lang/IllegalMonitorStateException if the current thread
    // is not the owner of the lock.

    VM_RT_MONITOR_EXIT_NON_NULL=305,
    // Arguments:
    //  - Object
    // Return value: none.
    //
    // Release the monitor associated with the object.
    // This function is identical to VM_RT_MONITOR_EXIT except that it
    // assumes that the argument is non-null.  Passing a null argument
    // will result in undefined behavior.
    // Throw java/lang/IllegalMonitorStateException if the current thread
    // is not the owner of the lock.

    VM_RT_MONITOR_ENTER_STATIC=302,
    // Arguments:
    //  - Class handle
    // Return value: none.
    //
    // Acquire the monitor associated with the class passed as the argument.

    VM_RT_MONITOR_EXIT_STATIC=303,
    // Arguments:
    //  - Class handle
    // Return value: none.
    //
    // Release the monitor associated with the class passed as the argument.
    // Throw java/lang/IllegalMonitorStateException if the current thread
    // is not the owner of the lock.




    //////////////////////////////////////////////////////////////////////////
    // Type access routines
    //////////////////////////////////////////////////////////////////////////

    VM_RT_CHECKCAST=400,
    // Arguments:
    //  - Object
    //  - Class
    // Return value:
    //  - Object
    //
    // If object can't be cast to class, throw ClassCastException


    VM_RT_INSTANCEOF=401,
    // Arguments:
    //  - Object
    //  - Class
    // Return value:
    //  - 1 if object is not null and can be cast to class
    //  - 0 otherwise

    VM_RT_AASTORE=402,
    // Arguments:
    //  - Element
    //  - Index
    //  - Array
    // Return value: none.
    //
    // Store a reference element into into an array.

    VM_RT_AASTORE_TEST=403,
    // Arguments:
    //  - Element
    //  - Array
    // Return value: 1 if the element can be stored in the array, 0 otherwise.
    //
    // Check if a reference element into into an array.  If the 'array'
    // argument is null, return 0.  This function does not throw exceptions.

    VM_RT_GET_INTERFACE_VTABLE_VER0=404,
    // Arguments:
    //  - Object reference
    //  - Class handle for the interface
    // Return value:
    //  - Interface vtable with method entries for the implementation
    //    of that interface by the actual class of the object.




    //////////////////////////////////////////////////////////////////////////
    // Class initialization
    //////////////////////////////////////////////////////////////////////////

    VM_RT_INITIALIZE_CLASS=500,
    // Arguments:
    //  - Class handle
    // Return value: none.
    //
    // If the class hasn't been initialized yet, initialize it.
    //
    // A call to this function must be generated before every putstatic
    // and getstatic unless the JIT can prove that the class would have
    // been already initialized at that point.




    //////////////////////////////////////////////////////////////////////////
    // Write barrier routines
    //////////////////////////////////////////////////////////////////////////

    VM_RT_GC_HEAP_WRITE_REF=600,
    // Arguments:
    //  - Address of the base of the object (or array) being written to.
    //  - Address of a memory location being written to
    //  - Value being written.
    // Return value: none.
    //
    // Write barrier for GC. Updates the slot with the value provided.
    // The type being written is indicated in the name.

    VM_RT_GC_SAFE_POINT=601,
    // Arguments: none
    // Return value: none.
    // Use this helper to notify GC about safe point in managed code

    VM_RT_GC_GET_THREAD_SUSPEND_FLAG_PTR=602,
    // Arguments: none
    // Return value: pointer to int-sized suspension request flag
    //
    // When the flag is non zero, managed code should call VM_RT_GC_SAFE_POINT
    // helper.
    // Note: The flag must be used for read only !

    //////////////////////////////////////////////////////////////////////////
    // JVMTI specific routines
    //////////////////////////////////////////////////////////////////////////

    VM_RT_JVMTI_METHOD_ENTER_CALLBACK = 700,
    // Arguments: handle of the method which gets control
    // Return value: none
    //
    // This call-back should be used to notify about method enter event.
    // Do a call-back when such capability is requested only.

    VM_RT_JVMTI_METHOD_EXIT_CALLBACK = 701,
    // Arguments:
    //  - handle of the method which is about to lose control
    //  - method's return value
    // Return value: none
    //
    // This call-back should be used to notify about method exit event.
    // Do a call-back when such capability is requested only.

    VM_RT_JVMTI_FIELD_ACCESS_CALLBACK = 702,
    // Arguments:
    //  - handle of the field under access
    //  - handle of the method, which accesses field
    //  - location of code which accesses field
    //  - pointer to the reference of the object, which field is beeng
    //    accessed or NULL for static field
    // Return value: none
    //
    // Notifies about field access.

    VM_RT_JVMTI_FIELD_MODIFICATION_CALLBACK = 703,
    // Arguments:
    // - handle of the field under modification
    // - handle of the method, which modifies field
    // - location of code which modifies field
    // - pointer to the reference of the object, which field is beeng
    //   modified or NULL for static field
    // - pointer to the new value for the field
    // Return value: none
    //
    // Notifies about field modification.

    //////////////////////////////////////////////////////////////////////////
    // Runtime resolution routine
    //////////////////////////////////////////////////////////////////////////

    VM_RT_RESOLVE = 801,
    // Arguments:
    //  - Class handle
    //  - Constant pool index of an item to be resolved
    //  - A java byte code instruction (JavaByteCodes) the resolution
    //    is performed for
    //
    // The helper performs resolution of symbolic references during runtime.
    // If resolution failed, the helper throws appropriate LinkageError.
    // Otherwise, the value depends on byte code passed:
    //
    //  === JavaByteCodes ===          === result of call ===
    //  OPCODE_INVOKESTATIC:        resolve_static_method
    //  OPCODE_INVOKEINTERFACE:     resolve_interface_method
    //  OPCODE_INVOKEVIRTUAL,
    //  OPCODE_INVOKESPECIAL:       resolve_virtual_method
    //  OPCODE_INSTANCEOF,
    //  OPCODE_CHECKCAST,
    //  OPCODE_MULTIANEWARRAY       resolve_class
    //  OPCODE_ANEWARRAY:           class_get_array_of_class(resolve_class())
    //  OPCODE_NEW:                 resolve_class_new
    //  OPCODE_GETFIELD,
    //  OPCODE_PUTFIELD:            resolve_nonstatic_field
    //  OPCODE_PUTSTATIC,
    //  OPCODE_GETSTATIC:           resolve_static_field
    //
    // The helper initializes class if the class hasn't been initialized yet
    // for PUTSTATIC and GETSTATIC
    //

    //////////////////////////////////////////////////////////////////////////
    // Non-VM specific helpers for the JIT
    //////////////////////////////////////////////////////////////////////////

    VM_RT_F2I=900,
    VM_RT_F2L=901,
    VM_RT_D2I=902,
    VM_RT_D2L=903,
    // Conversions


    VM_RT_LSHL=910,
    VM_RT_LSHR=911,
    VM_RT_LUSHR=912,
    // Arguments:
    // (new version)
    //  - Long value to be shifted
    //  - Int value with count
    // (old version)
    //  - (edx:eax) A long value to be shifted
    //  - (ecx)     Count 
    // Return value: (edx:eax)
    //  - The shifted value
    //


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
    // Arguments:
    //  - Object reference for the source array. Must be non-null and refer to an array 
    //    of 16 bit chars.
    //  - int32 containing the starting index of the source array to copy.
    //  - Object reference for the destination array. Must be non-null and refer to an array 
    //    of 16 bit chars.
    //  - int32 containing the starting index of the destination array to copy into.
    //  - int32 containing the number of 16 bit chars to copy.
    // Return value: none.
    // 
    // This runtime helper function provides a specialized implementation for System.arraycopy
    // for the common case where the elements of the arrays are 
    // 16 bit characters and the array references are both non-null. The JIT must guarantee
    // that no exceptions will need to be thrown. That is, both array references are non-null,
    // the types match (and are both arrays of 16 bit characters), 
    // and the starting indexes and length are such that the copying will only access
    // data in the bounds of the arrays.



    //////////////////////////////////////////////////////////////////////////
    // Deprecated routines
    //////////////////////////////////////////////////////////////////////////

    VM_RT_NEW_RESOLVED=1000,
    // Arguments:
    //  - Class handle for the class whose object we want to create
    // Return value:
    //  - Reference to the new object
    //
    // Allocates a new object of the class.  This function should be used
    // for classes which inherit the finalize method of java.lang.Object.
    // See also VM_RT_NEW_RESOLVED_USING_VTABLE.

    VM_RT_NEW_VECTOR=1001,
    // Arguments:
    //  - Vector length
    //  - Class handle of the vector class
    // Return value:
    //  - Reference to the new object
    //
    // Create a vector (zero-based, one dimensional array)
    // of the given type. See also VM_RT_NEW_VECTOR_USING_VTABLE.

    VM_RT_WRITE_BARRIER_FASTCALL=1002
    // Arguments:
    //  - Address of a memory location being written to
    //  - Object reference being written.
    // Return value: none.
    //
    // Write barrier for GC.

} VM_RT_SUPPORT; //VM_RT_SUPPORT



NativeCodePtr rth_get_lil_helper(VM_RT_SUPPORT f);

VMEXPORT void *vm_get_rt_support_addr(VM_RT_SUPPORT f);


/* temporary interface addition
 * same as vm_get_rt_support_addr, but tries to optimize the stubs it creates,
 * by specializing them.
 */
VMEXPORT void *vm_get_rt_support_addr_optimized(VM_RT_SUPPORT f, Class_Handle c);


// instead of returning a stub address, this support function returns
// parsed LIL code.
typedef struct LilCodeStub LilCodeStub;
VMEXPORT LilCodeStub *vm_get_rt_support_stub(VM_RT_SUPPORT f, Class_Handle c);


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !_JIT_RUNTIME_SUPPORT_H_
