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
 * @author Pavel Pervov
 * @version $Revision: 1.1.2.3.4.4 $
 */  

#define LOG_DOMAIN util::CLASS_LOGGER
#include "cxxlog.h"

#include "environment.h"
#include "Package.h"
#include "String_Pool.h"
#include "Class.h"
#include "nogc.h"
#include "GlobalClassLoaderIterator.h"

#include "verifier.h"
#include "native_overrides.h"
#include "compile.h"
#include "component_manager.h"


Global_Env::Global_Env(apr_pool_t * pool):
mem_pool(pool),
bootstrap_class_loader(NULL),
system_class_loader(NULL),
properties(NULL),
TI(NULL),
nsoTable(NULL),
portLib(NULL),
dcList(NULL),
assert_reg(NULL),
vm_methods(NULL),
bootstrapping(false),
ready_for_exceptions(false)
{
    // TODO: Use proper MM.
    properties = new Properties();
    bootstrap_class_loader = new BootstrapClassLoader(this); 

    hythread_lib_create(&hythread_lib);

#if defined _IPF_ || defined _EM64T_
    compact_fields = true;
    sort_fields = true;
#else // !_IPF_
    compact_fields = false;
    sort_fields = false;
#endif // !IPF_
    heap_base = heap_end = managed_null = NULL;

    JavaLangString_String = string_pool.lookup("java/lang/String");
    JavaLangStringBuffer_String = string_pool.lookup("java/lang/StringBuffer");
    JavaLangObject_String = string_pool.lookup("java/lang/Object");
    JavaLangClass_String = string_pool.lookup("java/lang/Class");
    VoidVoidDescriptor_String = string_pool.lookup("()V");
    VoidBooleanDescriptor_String = string_pool.lookup("()Z");
    VoidIntegerDescriptor_String = string_pool.lookup("()I");
    JavaLangThrowable_String = string_pool.lookup("java/lang/Throwable");
    JavaLangNoClassDefFoundError_String = string_pool.lookup("java/lang/NoClassDefFoundError");

    JavaLangArrayIndexOutOfBoundsException_String = string_pool.lookup("java/lang/ArrayIndexOutOfBoundsException");
    JavaNioByteBuffer_String = string_pool.lookup("java/nio/ByteBuffer");
    JavaLangIllegalArgumentException_String = string_pool.lookup("java/lang/IllegalArgumentException");
    JavaLangReflectField_String = string_pool.lookup("java/lang/reflect/Field");
    JavaLangReflectConstructor_String = string_pool.lookup("java/lang/reflect/Constructor");
    JavaLangReflectMethod_String = string_pool.lookup("java/lang/reflect/Method");
    JavaLangUnsatisfiedLinkError_String = string_pool.lookup("java/lang/UnsatisfiedLinkError");
    JavaLangNullPointerException_String = string_pool.lookup("java/lang/NullPointerException");


    Init_String = string_pool.lookup("<init>");
    Clinit_String = string_pool.lookup("<clinit>");
    FinalizeName_String = string_pool.lookup("finalize");
    EnqueueName_String = string_pool.lookup("enqueue");
    Clonable_String = string_pool.lookup("java/lang/Cloneable");
    Serializable_String = string_pool.lookup("java/io/Serializable");

    Length_String = string_pool.lookup("length");
    LoadClass_String = string_pool.lookup("loadClass");
    InitCause_String = string_pool.lookup("initCause");
    FromStringConstructorDescriptor_String = string_pool.lookup("(Ljava/lang/String;)V");
    LoadClassDescriptor_String = string_pool.lookup("(Ljava/lang/String;)Ljava/lang/Class;");
    InitCauseDescriptor_String = string_pool.lookup("(Ljava/lang/Throwable;)Ljava/lang/Throwable;");

    //
    // globals
    //

    // HT support from command line disabled. VM should automatically detect HT status at startup.
    is_hyperthreading_enabled = false;
    use_lil_stubs = true;
#if defined _IPF_ || defined _EM64T_
    compress_references = true;
#else // !_IPF_
    compress_references = false;
#endif // !_IPF_

    strings_are_compressed = false;
    use_large_pages = false;
    verify_all = false;
    pin_interned_strings = false; 

    // initialize critical sections
    p_jit_a_method_lock = new Lock_Manager();
    p_vtable_patch_lock = new Lock_Manager();
    p_meth_addr_table_lock = new Lock_Manager();
    p_handle_lock = new Lock_Manager();
    p_method_call_lock = new Lock_Manager();

    //
    // preloaded classes
    //
    Boolean_Class = NULL;
    Char_Class = NULL;
    Float_Class = NULL;
    Double_Class = NULL;
    Byte_Class = NULL;
    Short_Class = NULL;
    Int_Class = NULL;
    Long_Class = NULL;
    ArrayOfBoolean_Class = NULL;
    ArrayOfChar_Class = NULL;
    ArrayOfFloat_Class = NULL;
    ArrayOfDouble_Class = NULL;
    ArrayOfByte_Class = NULL;
    ArrayOfShort_Class = NULL;
    ArrayOfInt_Class = NULL;
    ArrayOfLong_Class = NULL;
    JavaLangObject_Class = NULL;
    JavaLangString_Class = NULL;
    JavaLangClass_Class = NULL;
    java_lang_Throwable_Class = NULL;
    java_lang_Error_Class = NULL;
    java_lang_ThreadDeathError_Class = NULL;
    java_lang_ExceptionInInitializerError_Class = NULL;
    java_lang_NullPointerException_Class = NULL;
    java_lang_StackOverflowError_Class = NULL;
    java_lang_ArrayIndexOutOfBoundsException_Class = NULL;
    java_lang_ArrayStoreException_Class = NULL;
    java_lang_ArithmeticException_Class = NULL;
    java_lang_ClassCastException_Class = NULL;
    java_lang_OutOfMemoryError_Class = NULL;

    java_lang_OutOfMemoryError = NULL;
    popFrameException = NULL;

    java_io_Serializable_Class = NULL;
    java_lang_Cloneable_Class = NULL;
    java_lang_Thread_Class = NULL;
    java_lang_ThreadGroup_Class = NULL;

    java_lang_reflect_Constructor_Class = NULL;
    java_lang_reflect_Field_Class = NULL;
    java_lang_reflect_Method_Class = NULL;

    JavaLangString_VTable = NULL;
    JavaLangString_allocation_handle = 0;

    vm_class_offset = 0;

    TI = new DebugUtilsTI; 
    vm_methods = new Method_Lookup_Table;

    nsoTable = nso_init_lookup_table(&string_pool);

} //Global_Env::Global_Env

Global_Env::~Global_Env()
{
    if (vm_get_boolean_property_value_with_default("vm.noCleanupOnExit")) {
        return;
    }

    GlobalClassLoaderIterator ClIterator;
    ClassLoader *cl = ClIterator.first();
    
    while(cl) {
        ClassLoader* cltmp = cl;
        cl = ClIterator.next();
        delete cltmp;
    }
    ClassLoader::DeleteClassLoaderTable();

    delete TI;
    TI = NULL;

    delete vm_methods;
    vm_methods = NULL;

    delete properties;
    properties = NULL;

    nso_clear_lookup_table(nsoTable);
    nsoTable = NULL;

    compile_clear_dynamic_code_list(dcList);
    dcList = NULL;

    // uninitialize critical sections
    delete p_jit_a_method_lock;
    delete p_vtable_patch_lock;
    delete p_meth_addr_table_lock;
    delete p_handle_lock;
    delete p_method_call_lock;

    // Unload jit instances.
    vm_delete_all_jits();

    // Unload component manager and all registered components.
    CmRelease();

    // Unload all system native libraries.
    natives_cleanup();

    // Unload GC-releated resources.
    gc_wrapup();

    // Deallocate global vtable & jit code pools.
    vm_mem_dealloc();

    // TODO: Currently, there is only one global thread library instance.
    // It still can be used after VM is destroyed.
    // hythread_lib_destroy(hythread_lib);
 }

Class* Global_Env::LoadCoreClass(const String* s)
{
    Class* clss =
        bootstrap_class_loader->LoadVerifyAndPrepareClass(this, s);
    if(clss == NULL) {
        // print error diagnostics and exit VM
        WARN("Failed to load bootstrap class " << s->bytes);
        LOGGER_EXIT(1);
    }
    return clss;
}

Class* Global_Env::LoadCoreClass(const char* s)
{
    return LoadCoreClass(this->string_pool.lookup(s));
}