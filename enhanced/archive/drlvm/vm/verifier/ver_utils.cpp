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
 * @author Pavel Rebriy
 * @version $Revision: 1.1.2.3.4.4 $
 */

#include "ver_real.h"
#include "port_malloc.h"

/**
 * Debug flag macros
 */
// Macro enable verifier memory trace
#define VF_TRACE_MEMORY 0

/************************************************************
 *********************** Hash class *************************
 ************************************************************/

/**
 * Hash table constructor.
 */
vf_Hash::vf_Hash ():HASH_SIZE(127), m_free(true)
{
    m_pool = vf_create_pool();
    m_hash = (vf_HashEntry **) vf_palloc(m_pool,
                                         HASH_SIZE * sizeof(vf_HashEntry *));
}                               // vf_Hash::vf_Hash

/**
 * Hash table constructor.
 */
vf_Hash::vf_Hash (vf_Pool *pool):HASH_SIZE(127), m_free(false)
{
    m_pool = pool;
    m_hash = (vf_HashEntry **) vf_palloc(m_pool,
                                         HASH_SIZE * sizeof(vf_HashEntry *));
}                               // vf_Hash::vf_Hash

/**
 * Hash table destructor.
 */
vf_Hash::~vf_Hash ()
{
    if (m_free) {
        vf_delete_pool(m_pool);
    }
}                               // vf_Hash::vf_Hash

/**
 * Checks key identity.
 */
inline bool vf_Hash::CheckKey(vf_HashEntry *hash_entry, // checked hash entry
                              const char *key)  // checked key
{
    if (!strcmp(hash_entry->key, key)) {
        return true;
    }
    return false;
}                               // vf_Hash::CheckKey( hash, key )

/**
 * Checks key identity.
 */
inline bool vf_Hash::CheckKey(vf_HashEntry *hash_entry, // checked hash entry
                              const char *key,  // checked key
                              size_t len)       // key length
{
    if (!strncmp(hash_entry->key, key, len)
        && hash_entry->key[len] == '\0') {
        return true;
    }
    return false;
}                               // vf_Hash::CheckKey( hash, key, len )

/**
 * Hash function.
 */
inline unsigned vf_Hash::HashFunc(const char *key)      // key for hash function
{
    unsigned result = 0;
    unsigned char ch;

    while ((ch = (unsigned char) (*key++))) {
        result = (result * 16777619) ^ ch;
    }
    return result;
}                               // vf_Hash::HashFunc( key )

/**
 * Hash function.
 */
inline unsigned vf_Hash::HashFunc(const char *key,      // key for hash function
                                  size_t len)   // key length
{
    unsigned result = 0;

    for (unsigned index = 0; index < len; index++) {
        result = (result * 16777619) ^ (unsigned char) (*key++);
    }
    return result;
}                               // vf_Hash::HashFunc( key, len )

/**
 * Function looks up hash entry which is identical to current key.
 */
inline vf_HashEntry *vf_Hash::Lookup(const char *key)   // hash key
{
    assert(key);
    unsigned hash_index = HashFunc(key);
    hash_index = hash_index % HASH_SIZE;
    vf_HashEntry *hash_entry = m_hash[hash_index];
    while (hash_entry != NULL) {
        if (CheckKey(hash_entry, key)) {
            return hash_entry;
        }
        hash_entry = hash_entry->next;
    }
    return NULL;
}                               // vf_Hash::Lookup( key )

/**
 * Function looks up hash entry which is identical to current key.
 */
inline vf_HashEntry *vf_Hash::Lookup(const char *key,   // hash key
                                     size_t len)        // key length
{
    assert(key);
    assert(len);
    unsigned hash_index = HashFunc(key, len);
    hash_index = hash_index % HASH_SIZE;
    vf_HashEntry *hash_entry = m_hash[hash_index];
    while (hash_entry != NULL) {
        if (CheckKey(hash_entry, key, len)) {
            return hash_entry;
        }
        hash_entry = hash_entry->next;
    }
    return NULL;
}                               // vf_Hash::Lookup( key, len )

/**
 * Function creates hash entry which is identical to current key.
 */
inline vf_HashEntry *vf_Hash::NewHashEntry(const char *key)     // hash key
{
    // lookup type in hash
    assert(key);
    unsigned hash_index = HashFunc(key);
    hash_index = hash_index % HASH_SIZE;
    vf_HashEntry *hash_entry = m_hash[hash_index];
    while (hash_entry != NULL) {
        if (CheckKey(hash_entry, key)) {
            return hash_entry;
        }
        hash_entry = hash_entry->next;
    }

    // create key string
    size_t len = strlen(key);
    char *hash_key = (char *) vf_palloc(m_pool, len + 1);
    memcpy(hash_key, key, len);

    // create hash entry
    hash_entry = (vf_HashEntry *) vf_palloc(m_pool, sizeof(vf_HashEntry));
    hash_entry->key = hash_key;
    hash_entry->next = m_hash[hash_index];
    m_hash[hash_index] = hash_entry;

    return hash_entry;
}                               // vf_Hash::NewHashEntry( key )

/**
 * Function creates hash entry which is identical to current key.
 */
inline vf_HashEntry *vf_Hash::NewHashEntry(const char *key,     // hash key
                                           size_t len)  // key length
{
    // lookup type in hash
    assert(key);
    assert(len);
    unsigned hash_index = HashFunc(key, len);
    hash_index = hash_index % HASH_SIZE;
    vf_HashEntry *hash_entry = m_hash[hash_index];
    while (hash_entry != NULL) {
        if (CheckKey(hash_entry, key, len)) {
            return hash_entry;
        }
        hash_entry = hash_entry->next;
    }

    // create key string
    char *hash_key = (char *) vf_palloc(m_pool, len + 1);
    memcpy(hash_key, key, len);

    // create hash entry
    hash_entry = (vf_HashEntry *) vf_palloc(m_pool, sizeof(vf_HashEntry));
    hash_entry->key = hash_key;
    hash_entry->next = m_hash[hash_index];
    m_hash[hash_index] = hash_entry;

    return hash_entry;
}                               // vf_Hash::NewHashEntry( key, len )

/************************************************************
 ******************** Valid Type class **********************
 ************************************************************/

/**
 * Type constraint collection constructor.
 */
vf_TypePool::vf_TypePool ()
:           m_method(NULL), m_name(NULL), m_descriptor(NULL),
m_restriction(NULL)
{
    m_pool = vf_create_pool();
    m_Hash = new vf_Hash (m_pool);
}                               // vf_TypePool::vf_TypePool

/**
 * Type constraint collection destructor.
 */
vf_TypePool::~vf_TypePool ()
{
    delete m_Hash;
    vf_delete_pool(m_pool);
}                               // vf_TypePool::vf_TypePool

/**
 * Function creates valid type which is identical to current class.
 */
vf_ValidType *vf_TypePool::NewType(const char *type,    // class name
                                   size_t len)  // name length
{
    vf_ValidType *result;
    vf_HashEntry *hash;

    // find type in hash
    hash = m_Hash->NewHashEntry(type, len);
    if (hash->data) {
        assert(((vf_ValidType *) hash->data)->number == 1);
        return (vf_ValidType *) hash->data;
    }
    // create and set type
    result = (vf_ValidType *) vf_palloc(m_pool, sizeof(vf_ValidType));
    result->number = 1;
    result->string[0] = hash->key;
    // set type in hash
    hash->data = result;

    return result;
}                               // vf_TypePool::NewType


static int
vf_type_string_compare(const void *type_string1, const void *type_string2)
{
    if (type_string1 < type_string2) {
        return -1;
    } else if (type_string1 > type_string2) {
        return 1;
    } else {
        return 0;
    }
}                               // vf_type_string_compare

/**
 * Function creates valid type which is identical to an element of a given array type.
 */
vf_ValidType *vf_TypePool::NewArrayElemType(vf_ValidType *array)        // array type
{
    vf_ValidType *result;
    vf_HashEntry *hash;

    // lookup type in hash
    hash = m_Hash->NewHashEntry(&array->string[0][1]);
    if (array->number == 1 && hash->data) {
        assert(((vf_ValidType *) hash->data)->number == 1);
        return (vf_ValidType *) hash->data;
    }
    // create and set type
    result = (vf_ValidType *) vf_palloc(m_pool,
                                        sizeof(vf_ValidType) +
                                        (array->number -
                                         1) * sizeof(const char *));
    result->number = array->number;
    // set sting type
    result->string[0] = hash->key;
    if (result->number == 1) {
        // set type in hash
        hash->data = result;
    } else {
        // set other string types
        for (unsigned index = 1; index < array->number; index++) {
            hash = m_Hash->NewHashEntry(&array->string[index][1]);
            result->string[index] = hash->key;
        }
        // sort valid type
        qsort(&result->string[0], result->number, sizeof(const char *),
              vf_type_string_compare);
    }

    return result;
}                               // vf_TypePool::NewType

/**
 * Dumps constraint collection in stream.
 */
void vf_TypePool::DumpTypeConstraints(ostream *out)     // output stream
{
    if (out == NULL) {
        out = &cerr;
    }
    for (vf_TypeConstraint *constraint = m_restriction;
         constraint; constraint = constraint->m_next) {
        *out << "CONSTRAINT: have \""
            << constraint->m_source << "\" need \""
            << constraint->m_target << "\" for method "
            << class_get_name(method_get_class(m_method)) << "."
            << m_name << m_descriptor << endl;
    }
}                               // vf_TypePool::DumpTypeConstraints

/**
 * Function returns the methods constraints array.
 */
inline vf_TypeConstraint *vf_TypePool::GetClassConstraint()
{
    return m_restriction;
}                               // vf_TypePool::GetClassConstraint

/**
 * Sets current context method.
 */
void vf_TypePool::SetMethod(vf_ContextHandle ctx)
{
    // set method context
    m_method = ctx->m_method;
    m_name = ctx->m_name;
    m_descriptor = ctx->m_descriptor;
}                               // vf_TypePool::SetMethod

/**
 * Sets restriction from target class to source class.
 */
void vf_TypePool::SetRestriction(const char *target,    // target class name
                                 const char *source,    // source class name
                                 unsigned short index,  // constant pool index
                                 vf_CheckConstraint check_type) // constraint check type
{
    vf_TypeConstraint *restriction;

    // lookup restriction
    for (restriction = m_restriction;
         restriction; restriction = restriction->m_next) {
        if (restriction->m_target == target
            && restriction->m_source == source
            && restriction->m_index == index
            && restriction->m_check_type == check_type) {
            // this restriction is already present
            return;
        }
    }

    // set restriction
    restriction = (vf_TypeConstraint *) vf_palloc(m_pool,
                                                  sizeof(vf_TypeConstraint));
    restriction->m_target = target;
    restriction->m_source = source;
    restriction->m_index = index;
    restriction->m_check_type = check_type;
    restriction->m_method = m_method;
    restriction->m_name = m_name;
    restriction->m_descriptor = m_descriptor;
    restriction->m_next = m_restriction;
    m_restriction = restriction;

    // trace restriction
    if (index) {
        VF_TRACE("constraint", "CONSTRAINT: for class \""
                 << class_get_name(method_get_class(m_method))
                 << "\" CP index #" << index << " check access: have \""
                 << source << "\" need \"" << target << "\" for method "
                 << class_get_name(method_get_class(m_method)) << "."
                 << m_name << m_descriptor);
    } else {
        VF_TRACE("constraint", "CONSTRAINT: have \""
                 << source << "\" need \"" << target << "\" for method "
                 << class_get_name(method_get_class(m_method)) << "."
                 << m_name << m_descriptor);
    }
}                               // vf_TypePool::SetRestriction

/**
 * Checks types and create constraints if it's necessarily.
 */
bool vf_TypePool::CheckTypes(vf_ValidType *required,    // required type
                             vf_ValidType *available,   // available type
                             unsigned short index,      // constant pool index
                             vf_CheckConstraint check_type)     // constraint check type
{
    unsigned index1, index2;

    switch (check_type) {
    case VF_CHECK_ARRAY:       // provide array check
        // check if available types are array
        for (index1 = 0; index1 < available->number; index1++) {
            if (available->string[index1][0] != '[') {
                // type isn't array, return error
                return true;
            }
        }
        return false;

    case VF_CHECK_REF_ARRAY:   // provide reference array check
        for (index1 = 0; index1 < available->number; index1++) {
            if (available->string[index1][0] == '['
                && (available->string[index1][1] == '['
                    || available->string[index1][1] == 'L')) {
                // type is reference array, continue loop
                continue;
            } else {
                // type isn't array, return error
                return true;
            }
        }
        return false;

    case VF_CHECK_EQUAL:       // check equivalence
        // check if available types are equal
        return !vf_is_types_equal(required, available);

    case VF_CHECK_PARAM:       // check method invocation conversion
    case VF_CHECK_ASSIGN:      // check assignment conversion
    case VF_CHECK_ASSIGN_WEAK: // check weak assignment conversion
        // compare types
        for (index1 = 0; index1 < required->number; index1++) {
            for (index2 = 0; index2 < available->number; index2++) {
                // set constraint for differing types
                if (required->string[index1] != available->string[index2]) {
                    SetRestriction(required->string[index1],
                                   available->string[index2], 0, check_type);
                }
            }
        }
        return false;

    case VF_CHECK_ACCESS_FIELD:        // check field access
        // compare types
        assert(required->number == 1);
        for (index1 = 0; index1 < available->number; index1++) {
            // set access and type constraints for differing types
            if (required->string[0] != available->string[index1]) {
                SetRestriction(required->string[0],
                               available->string[index1], 0, VF_CHECK_ASSIGN);
            }
            SetRestriction(required->string[0], available->string[index1],
                           index, VF_CHECK_ACCESS_FIELD);
        }
        return false;

    case VF_CHECK_ACCESS_METHOD:       // check method access
        // compare types
        assert(required->number == 1);
        for (index1 = 0; index1 < available->number; index1++) {
            // set access and type constraints for differing types
            if (required->string[0] != available->string[index1]) {
                SetRestriction(required->string[0],
                               available->string[index1], 0, VF_CHECK_PARAM);
            }
            SetRestriction(required->string[0], available->string[index1],
                           index, VF_CHECK_ACCESS_METHOD);
        }
        return false;
    case VF_CHECK_DIRECT_SUPER:
        if (required->number == 1 && available->number == 1) {
            if (required->string[0] != available->string[0]) {
                SetRestriction(required->string[0], available->string[0],
                               0, VF_CHECK_DIRECT_SUPER);
            }
            return false;
        }
        return true;

    case VF_CHECK_INVOKESPECIAL:
        assert(required->number == 1);
        for (index1 = 0; index1 < available->number; index1++) {
            SetRestriction(required->string[0], available->string[index1],
                           index, VF_CHECK_INVOKESPECIAL);
        }
        return false;
    default:
        VF_DIE("CompareTypes: unknown check type in switch");
        return true;
    }
    // unreachable instr
    assert(0);
}                               // vf_TypePool::CheckTypes

/**
 * Function merges two valid types.
 * Function returns NULL if vector wasn't merged.
 */
vf_ValidType *vf_TypePool::MergeTypes(vf_ValidType *first,      // first merged type
                                      vf_ValidType *second)     // second merged type
{
    // check null reference
    if (first == NULL) {
        return second;
    } else if (second == NULL) {
        return first;
    }
    // count differ types
    unsigned index1;
    unsigned index2;
    unsigned last_found;
    unsigned count = first->number + second->number;
    for (index1 = last_found = 0; index1 < first->number; index1++) {
        // find first type in second types
        for (index2 = last_found; index2 < second->number; index2++) {
            if (first->string[index1] == second->string[index2]) {
                // found first type
                last_found = index2 + 1;
                count--;
                break;
            } else if (first->string[index1] < second->string[index2]) {
                break;
            }
        }
    }
    if (first->number == second->number && count == first->number) {
        // types are equal, no need to merge
        return NULL;
    }
    // create merged type
    vf_ValidType *type = (vf_ValidType *) vf_palloc(m_pool,
                                                    sizeof(vf_ValidType)
                                                    + (count -
                                                       1) *
                                                    sizeof(const char *));
    type->number = count;

    // set type in ascending order of types string
    index1 = index2 = 0;
    for (unsigned index = 0; index < count; index++) {
        if (index1 >= first->number) {
            type->string[index] = second->string[index2++];
        } else if (index2 >= second->number) {
            type->string[index] = first->string[index1++];
        } else if (first->string[index1] < second->string[index2]) {
            type->string[index] = first->string[index1++];
        } else if (first->string[index1] > second->string[index2]) {
            type->string[index] = second->string[index2++];
        } else {
            type->string[index] = first->string[index1++];
            index2++;
        }
    }
    return type;
}                               // vf_TypePool::MergeTypes

/************************************************************
 ***************** Constraints functions ********************
 ************************************************************/

/**
 * Sets constraint for method in class loader verify data.
 */
static void vf_set_class_constraint(vf_TypeConstraint *collection,      // constraint for the class
                                    vf_ContextHandle ctx)       // verification context
{
    // get class loader of current class
    classloader_handler class_loader = class_get_class_loader(ctx->m_class);

    // lock data modification
    cl_acquire_lock(class_loader);
    vf_ClassLoaderData *cl_data =
        (vf_ClassLoaderData *) cl_get_verify_data_ptr(class_loader);

    // create class loader data
    if (cl_data == NULL) {
        vf_Pool *new_pool = vf_create_pool();
        cl_data = (vf_ClassLoaderData *) vf_palloc(new_pool,
                                                   sizeof
                                                   (vf_ClassLoaderData));
        cl_data->pool = new_pool;
        cl_data->string = new vf_Hash (new_pool);
        // set verify data for class loader
        cl_set_verify_data_ptr(class_loader, cl_data);
    }
    vf_Pool *pool = cl_data->pool;
    vf_Hash *hash = cl_data->string;

    // create class constraints collection
    vf_TypeConstraint *save_collection = NULL;
    for (vf_TypeConstraint *restriction = collection;
         restriction; restriction = restriction->m_next) {
        // create constraint
        vf_TypeConstraint *constraint = (vf_TypeConstraint *) vf_palloc(pool,
                                                                        sizeof
                                                                        (vf_TypeConstraint));
        // create entry in string pool for target class
        vf_HashEntry *hash_entry = hash->NewHashEntry(restriction->m_target);
        constraint->m_target = hash_entry->key;
        // create entry in string pool for checked class
        hash_entry = hash->NewHashEntry(restriction->m_source);
        constraint->m_source = hash_entry->key;
        // set only method name and descriptor due to
        // method handle could be changed during class preparation phase
        constraint->m_name = restriction->m_name;
        constraint->m_descriptor = restriction->m_descriptor;
        constraint->m_check_type = restriction->m_check_type;
        constraint->m_next = save_collection;
        save_collection = constraint;
    }
    assert(save_collection);

    // save method verify data
    assert(class_get_verify_data_ptr(ctx->m_class) == NULL);
    class_set_verify_data_ptr(ctx->m_class, save_collection);

    // unlock data modification
    cl_release_lock(class_loader);
}                               // vf_set_class_constraint

/**
 * Receives class by given class name, loads it if it's needed.
 */
static class_handler vf_resolve_class(const char *name, // resolved class name
                                      bool need_load,   // load flag
                                      vf_ContextHandle ctx)     // verification context
{
    // get class name
    if (name[0] == 'L') {
        // class is class, skip 'L'
        name++;
    } else {
        // class is array
        unsigned index = 0;
        do {
            index++;
        } while (name[index] == '[');
        if (name[index] == 'L') {
            // array of objects, construct array name
            size_t len = strlen(name);
            char *buf = (char *) STD_ALLOCA(len + 2);
            memcpy(buf, name, len);
            buf[len] = ';';
            buf[len + 1] = '\0';
            name = buf;
        }
    }

    // get class loader
    classloader_handler class_loader = class_get_class_loader(ctx->m_class);

    // receive class
    class_handler result;
    if (need_load) {
        // trace verifier loads
        VF_TRACE("load", "verify load: class " << name);
        result = cl_load_class(class_loader, name);
    } else {
        result = cl_get_class(class_loader, name);
    }

    return result;
}                               // vf_resolve_class

/**
 * Checks if target class is super class of source class. 
 */
static inline bool vf_is_super_class(class_handler source,      // checked class
                                     class_handler target)      // super class
{
    // check super class
    for (; source; source = class_get_super_class(source)) {
        if (class_is_same_class(source, target)) {
            return true;
        }
    }
    return false;
}                               // vf_is_super_class

/**
 * Checks if target class is super interface of source class. 
 */
static inline bool vf_is_super_interface(class_handler source,  // checked class
                                         class_handler target)  // super interface
{
    assert(class_is_interface_(target));
    // classes are equal
    if (class_is_same_class(source, target)) {
        return true;
    }
    // check super interface
    for (; source; source = class_get_super_class(source)) {
        for (unsigned index = 0;
             index < class_get_superinterface_number(source); index++) {
            if (vf_is_super_interface
                (class_get_superinterface
                 (source, (unsigned short) index), target)) {
                return true;
            }
        }
    }
    return false;
}                               // vf_is_super_interface

/**
 * Checks method invocation conversion between source ans target classes.
 */
static bool vf_is_param_valid(class_handler source,     // checked class
                              class_handler target)     // required class
{
    // check if target class is array
    if (class_is_array(target) && class_is_array(source)) {
        // get array element classes
        return vf_is_param_valid(class_get_array_element_class(source),
                                 class_get_array_element_class(target));
    }
    // check widening reference conversions
    if (class_is_interface_(target)) {
        // target class is interface
        return vf_is_super_interface(source, target);
    } else {
        // target class is class
        return vf_is_super_class(source, target);
    }
}                               // vf_is_param_valid

/**
 * Checks narrow reference conversion between interface classes.
 * If class1 has method1 and class2 has method2, correspondingly,
 * and methods are the same name and signature, function returns 1.
 * If methods have different return types, function returns 0.
 */
static inline bool vf_check_interface_methods(class_handler class1,     // first interface class
                                              class_handler class2)     // second interface class
{
    assert(class_is_interface_(class1));
    assert(class_is_interface_(class2));

    // check interfaces methods    
    for (unsigned index = 0; index < class_get_method_number(class2); index++) {
        method_handler method1 = class_get_method(class2, index);
        for (unsigned count = 0; count < class_get_method_number(class1);
             count++) {
            method_handler method2 = class_get_method(class1, count);
            if (!strcmp(method_get_name(method1), method_get_name(method2))) {
                // interfaces have methods with the same name
                const char *sig1 = method_get_descriptor(method1);
                const char *sig2 = method_get_descriptor(method2);
                char *end_params = (char *) strrchr(sig1, ')');
                unsigned len = (unsigned) (end_params - sig1 + 1);
                if (!memcmp(sig1, sig2, len)) {
                    // methods arguments are the same
                    if (strcmp(&sig1[len], &sig2[len])) {
                        // methods have different return types
                        return false;
                    }
                }
            }

        }
    }
    return true;
}                               // vf_check_interface_methods

/**
 * Checks casting conversion between classes.
 */
static bool vf_is_checkcast_valid(class_handler source, // checked class
                                  class_handler target) // required class
{
    // check if target class and source class are array
    if (class_is_array(target) && class_is_array(source)) {
        // get array element classes
        return vf_is_checkcast_valid(class_get_array_element_class(source),
                                     class_get_array_element_class(target));
    }
    // check widening reference conversions
    if (vf_is_param_valid(source, target)) {
        return true;
    }
    // check narrowing reference conversions
    if (class_is_interface_(source)) {
        // source class is interface
        if (class_is_interface_(target)) {
            // target class is interface
            return vf_check_interface_methods(source, target);
        } else {
            // target class is class
            if (class_is_final_(target)) {
                // target class is final
                return vf_is_super_interface(target, source);
            } else {
                // target class isn't final
                return true;
            }
        }
    } else {
        // source class is class
        if (!memcmp(class_get_name(source), "java/lang/Object", 17)) {
            // source class is Object
            return true;
        }
        if (class_is_interface_(target)) {
            // target class is interface
            if (!class_is_final_(source)) {
                // super class isn't final
                return true;
            }
        } else {
            // target class is class
            return vf_is_super_class(target, source);
        }
    }
    return false;
}                               // vf_is_checkcast_valid

/**
 * Checks assignment conversion between classes.
 *
 * If strict flag is true, strict assignment compatible check is provided,
 * else if strict flag is false, weak assignment compatible check is provided.
 *
 * Strict assignment compatible check is provided for elements of array.
 * For weak assignment strict compatible check is provided for runtime by
 * execution engine.
 *
 * Here is an example of weak and strict assignment compatible check:
 *
 * ------------------------- weak check -----------------------------
 *
 *     Class1[] instance = new Class2[n];
 *     instance[0] = function_return_Class1();
 *     
 *     Class1 function_return_Class1() {
 *         return new Class2();
 *     }
 * 
 * Where Class2 extends or implements Class1.
 *
 * === bytecode ===
 *
 *   0: iconst_5
 *   1: anewarray #2=<Class Class2>
 *   4: astore_1
 *   5: aload_1
 *   6: iconst_0
 *   7: invokestatic #3=<Method test_weak1.function_return_Class1 ()Class1>
 *  10: aastore //-------------> weak assignment compatible check
 *  11: return
 *
 * Test is valid.
 *
 * ------------------------ strict check ----------------------------
 *
 *     Class1[][] instance = new Class2[n][];
 *     instance[0] = function_return_ArrayClass1();
 *     
 *     static Class1[] function_return_ArrayClass1() {
 *         return new Class2[5];
 *     }
 *
 * === bytecode ===
 *
 *   0: iconst_5
 *   1: anewarray #2=<Class [LClass2;>
 *   4: astore_1
 *   5: aload_1
 *   6: iconst_0
 *   7: invokestatic #3=<Method test_weak1.function_return_ArrayClass1 ()Class1[]>
 *  10: aastore //-------------> strict assignment compatible check
 *  11: return
 *
 * Test is invalid.
 */
static bool vf_is_assign_valid(class_handler source,    // checked class
                               class_handler target,    // required class
                               bool is_strict)  // strict condition flag
{
    // check assignment reference conversions
    if (class_is_array(source)) {
        // source class is array
        if (class_is_interface_(target)) {
            // target class is interface
            if (!memcmp(class_get_name(target), "java/lang/Cloneable", 20)
                || !memcmp(class_get_name(target), "java/io/Serializable",
                           21)) {
                // target class is Cloneable or Serializable
                return true;
            }
        } else {
            // target class is class
            if (!memcmp(class_get_name(target), "java/lang/Object", 17)) {
                // target class is object
                return true;
            } else if (class_is_array(target)) {
                // get array element classes
                return
                    vf_is_assign_valid(class_get_array_element_class
                                       (source),
                                       class_get_array_element_class
                                       (target), true);
            }
        }
    } else if (class_is_interface_(source)) {
        // source class is interface
        if (class_is_interface_(target)) {
            // target class is interface
            return vf_is_super_interface(source, target);
        } else {
            // target class is class
            if (!memcmp(class_get_name(target), "java/lang/Object", 17)) {
                // target class is object
                return true;
            }
            if (!is_strict) {
                /**
                 * Strict correspondence is used only for element array compare.
                 * Compare interface and class is weaker because it's impossible to
                 * create pure interface.
                 */
                return vf_is_super_interface(target, source);
            }
        }
    } else {
        // source class is class
        bool valid = vf_is_param_valid(source, target);
        if (!valid && !is_strict) {
            valid = vf_is_super_class(target, source);
        }
        return valid;
    }
    return false;
}                               // vf_is_assign_valid

/**
 * Checks conversions between classes.
 */
static bool vf_is_valid(class_handler source,   // checked class
                        class_handler target,   // required class
                        class_handler current,  // current class
                        unsigned check) // checked class type
{
    switch (check) {
    case VF_CHECK_PARAM:       // method invocation conversion
        return vf_is_param_valid(source, target);
    case VF_CHECK_ASSIGN:      // assignment conversion
        return vf_is_assign_valid(source, target, true);
    case VF_CHECK_ASSIGN_WEAK: // weak assignment conversion
        return vf_is_assign_valid(source, target, false);
    case VF_CHECK_CAST:        // casting conversion
        return vf_is_checkcast_valid(source, target);
    case VF_CHECK_SUPER:       // check if target is super class of source
        return vf_is_super_class(source, target);
    case VF_CHECK_DIRECT_SUPER:        // check if target is a direct super class of source
        return class_is_same_class(class_get_super_class(source), target);
    case VF_CHECK_ACCESS_FIELD:        // protected field access
    case VF_CHECK_ACCESS_METHOD:       // protected method access
        return vf_is_super_class(source, current);
    case VF_CHECK_INVOKESPECIAL:       // check object for invokespecial instruction
        return vf_is_super_class(source, current)
            && vf_is_super_class(current, target);
    }
    VF_DIE("vf_is_valid: invalid check type");
    return false;
}                               // vf_is_valid

/**
 * Sets verifier error.
 */
static inline void vf_set_error(unsigned check, // failed check
                                vf_Context *ctx)        // verification context
{
    switch (check) {
    case VF_CHECK_PARAM:
        VF_REPORT(ctx, "Incompatible argument for function");
        return;
    case VF_CHECK_ASSIGN:
        VF_REPORT(ctx, "Incompatible types for field assignment");
        return;
    case VF_CHECK_ASSIGN_WEAK:
        VF_REPORT(ctx, "Incompatible types for array assignment");
        return;
    case VF_CHECK_SUPER:
        VF_REPORT(ctx, "Exception class not a subclass of Throwable");
        return;
    case VF_CHECK_ACCESS_FIELD:
        VF_REPORT(ctx, "Bad access to protected field");
        return;
    case VF_CHECK_ACCESS_METHOD:
        VF_REPORT(ctx, "Bad access to protected method");
        return;
    case VF_CHECK_DIRECT_SUPER:
        VF_REPORT(ctx, "Call to wrong initialization method");
        return;
    case VF_CHECK_INVOKESPECIAL:
        VF_REPORT(ctx, "Incompatible object argument for invokespecial");
        return;
    }
    VF_DIE("vf_set_error: unknown check type");
}                               // vf_set_error

/**
 * Checks some constraints without loading of needed classes.
 */
static inline vf_Result vf_check_without_loading(vf_TypeConstraint *restriction,        // checked constraint
                                                 vf_Context *ctx)       // verification context
{
    switch (restriction->m_check_type) {
    case VF_CHECK_SUPER:
        /**
         * Extension for class java/lang/Object doesn't check
         * because it's expected all references extend it.
         */
        if (restriction->m_target == ctx->m_vtype.m_object->string[0]) {
            // no need to check
            return VF_OK;
        }
        break;

    case VF_CHECK_PARAM:
        /**
         * Extension for class java/lang/Object doesn't check
         * because it's expected all references extend it.
         */
        if (restriction->m_target == ctx->m_vtype.m_object->string[0]) {
            // no need to check
            return VF_OK;
        }

        /**
         * Extension for class [Ljava/lang/Object doesn't check
         * because it's expected all arrays extend it.
         * Just check is source array.
         */
        if (restriction->m_target == ctx->m_vtype.m_array->string[0]
            && restriction->m_source[0] == '['
            && (restriction->m_source[1] == '['
                || restriction->m_source[1] == 'L')) {
            // no need to check
            return VF_OK;
        }

        /**
         * Extension for java/lang/Cloneable and java/io/Serializable
         * interfaces doesn't check because it's expected all arrays extend it.
         * Just check is source array.
         */
        if ((restriction->m_target == ctx->m_vtype.m_clone->string[0]
             || restriction->m_target == ctx->m_vtype.m_serialize->string[0])
            && restriction->m_source[0] == '[') {
            // no need to check
            return VF_OK;
        }

        /**
         * If method invocation conversion takes place between array and
         * non-array reference, return error.
         */
        if ((restriction->m_target[0] != '['
             && restriction->m_source[0] == '[')
            || (restriction->m_target[0] != '['
                && restriction->m_source[0] == '[')) {
            vf_set_error(VF_CHECK_PARAM, ctx);
            return VF_ErrorIncompatibleArgument;
        }
        break;

    case VF_CHECK_ASSIGN_WEAK:
        // check assignment weak reference conversions
        if (restriction->m_source[0] == 'L') {
            return VF_OK;
        }
        assert(restriction->m_source[0] == '[');
        // go to the next check...
    case VF_CHECK_ASSIGN:
        // check assignment reference conversions
        if (restriction->m_source[0] == '[') {
            // source class is array
            if (!memcmp(restriction->m_target, "Ljava/lang/Cloneable", 21)
                || !memcmp(restriction->m_target, "Ljava/io/Serializable",
                           22)) {
                // target class is java/lang/Cloneable 
                // or java/lang/Serializable interface
                return VF_OK;
            } else if (restriction->m_target ==
                       ctx->m_vtype.m_object->string[0]) {
                // target class is java/lang/Object
                return VF_OK;
            } else if (restriction->m_target[0] != '[') {
                // target class isn't array class
                vf_set_error(restriction->m_check_type, ctx);
                return VF_ErrorIncompatibleArgument;
            }
        }
        break;

    default:
        break;
    }
    // need to load classes for check
    return VF_ClassNotLoaded;
}                               // vf_check_without_loading

/**
 * Checks constraint between loaded classes.
 * If any class isn't loaded, function returns unloaded error
 * to store restriction to the class for future constraint check.
 */
static inline vf_Result vf_check_constraint(vf_TypeConstraint *restriction,     // checked constraint
                                            vf_Context *ctx)    // verification context
{
    /**
     * Try to provide some checks without classes loading
     */
    if (!ctx->m_verify_all) {
        vf_Result result = vf_check_without_loading(restriction, ctx);
        if (result != VF_ClassNotLoaded) {
            // no need to check
            return result;
        }
    }
    // get target class handler
    class_handler target =
        vf_resolve_class(restriction->m_target, false, ctx);
    // get checked class
    class_handler source =
        vf_resolve_class(restriction->m_source, false, ctx);
    // check classes are loaded?
    if (!target || !source) {
        return VF_ClassNotLoaded;
    }

    /**
     * Verifier which is built on Java VM Specification 2nd Edition (4.9.2)
     * recommendation of verification process doesn't check interfaces usage.
     * Unfortunately, a lot of Java applications depends on this neglect.
     * To be compatible with those applications we should do full constraint
     * checks only if -Xverify:all option is present in command line.
     */
    if (!ctx->m_verify_all && class_is_interface_(target)) {
        // skip constraint check
        return VF_OK;
    }
    // check restriction
    if (!vf_is_valid(source, target, ctx->m_class, restriction->m_check_type)) {
        // return error
        vf_set_error(restriction->m_check_type, ctx);
        return VF_ErrorIncompatibleArgument;
    }
    return VF_OK;
}                               // vf_check_constraint

/**
 * Checks access to protected field/method.
 * If function cannot check constraint because of any class isn't loaded,
 * function return unloaded error to store restriction to the class
 * for future constraint check.
 */
vf_Result vf_check_access_constraint(const char *super_name,    // name of super class
                                     const char *instance_name, // name of instance class
                                     unsigned short index,      // constant pool index
                                     vf_CheckConstraint check_type,     // access check type
                                     vf_Context *ctx)   // verification context
{
    // get class handler of super class
    class_handler super_class = vf_resolve_class(super_name, false, ctx);
    if (!super_class || !vf_is_super_class(ctx->m_class, super_class)) {
        // obtained class isn't super class of a given class, no need to check
        return VF_OK;
    }
    // check if a class and a parent class is in the same package
    if (class_is_same_package(ctx->m_class, super_class)) {
        // class and parent class is in the same package,
        // no need check access to protect members
        return VF_OK;
    }
    // check is a member protected
    bool need_check = false;
    if (check_type == VF_CHECK_ACCESS_FIELD) {
        field_handler field =
            class_resolve_nonstatic_field(ctx->m_class, index);
        if (!field) {
            // NoSuchFieldError or IllegalAccessError - nothing to check
            VF_DEBUG("verifying class " << class_get_name(ctx->m_class)
                     << " (method " << ctx->m_name << ctx->m_descriptor
                     << ") couldn't resolve field with constant pool index #"
                     << index);
            return VF_OK;
        }
        if (field_is_protected(field)) {
            need_check = true;
        }
    } else {
        method_handler method = class_resolve_method(ctx->m_class, index);
        if (!method || method_is_static(method)) {
            // NoSuchMethodError or IllegalAccessError - nothing to check
            VF_DEBUG("verifying class " << class_get_name(ctx->m_class)
                     << " (method " << ctx->m_name << ctx->m_descriptor
                     << ") couldn't resolve method with constant pool index #"
                     << index);
            return VF_OK;
        }
        if (method_is_protected(method)) {
            if (instance_name[0] == '['
                && !memcmp(method_get_name(method), "clone", 6)) {
                // for arrays function clone is public
            } else {
                need_check = true;
            }
        }
    }
    if (!need_check) {
        // no need to check
        return VF_OK;
    }
    // get instance class
    class_handler instance = vf_resolve_class(instance_name, false, ctx);
    if (!instance) {
        // instance class isn't loaded
        return VF_ClassNotLoaded;
    }
    // check access constraint
    if (!vf_is_valid(instance, NULL, ctx->m_class, check_type)) {
        // return error
        vf_set_error(check_type, ctx);
        return VF_ErrorIncompatibleArgument;
    }
    return VF_OK;
}                               // vf_check_access_constraint

/**
 * Provides initial constraint checks for current class.
 * Checks only loaded classes, and stores restriction for unloaded ones.
 */
vf_Result vf_check_class_constraints(vf_Context *ctx)   // verification context
{
    // set class restriction collection
    vf_TypeConstraint *last = NULL;
    vf_TypeConstraint *collection = ctx->m_type->GetClassConstraint();

    // check constraints
    for (vf_TypeConstraint *constraint = collection;
         constraint; constraint = constraint->m_next) {
        // set context method
        ctx->m_method = constraint->m_method;
        ctx->m_name = constraint->m_name;
        ctx->m_descriptor = constraint->m_descriptor;

        // check constraint
        vf_Result result = vf_check_constraint(constraint, ctx);
        if (result == VF_OK) {
            // constraint checked, remove constraint from the collection
            if (!last) {
                collection = constraint->m_next;
            } else {
                last->m_next = constraint->m_next;
            }
        } else if (result != VF_ClassNotLoaded) {
            // return error
            return result;
        } else {
            // these constraints should be checked at the first time
            assert(constraint->m_check_type != VF_CHECK_ACCESS_FIELD);
            assert(constraint->m_check_type != VF_CHECK_ACCESS_METHOD);

            // set the last constraint
            last = constraint;
        }
    }
    if (collection) {
        // set constraint for further checking
        vf_set_class_constraint(collection, ctx);
    }

    return VF_OK;
}                               // vf_check_class_constraints

/**
 * Checks a constraint for a given class. Loads classes if it's needed.
 */
static inline vf_Result vf_force_check_constraint(vf_TypeConstraint *constraint,        // class constraint
                                                  vf_Context *ctx)      // verification context
{
    // check if constraint is already verified
    if (VF_CHECK_NONE == constraint->m_check_type) {
        // already verified
        VF_TRACE("constraint.checked", "verify constraint: have \""
                 << constraint->m_source << "\" need \"" << constraint->
                 m_target << "\" already done (check #1) for class " <<
                 class_get_name(ctx->m_class));
        return VF_OK;
    }
    // get target class
    class_handler target = vf_resolve_class(constraint->m_target, true, ctx);
    if (!target) {
        VF_DEBUG("verifying class " << class_get_name(ctx->m_class)
                 << " (method " << constraint->m_name
                 << constraint->m_descriptor << ") couldn't load class \""
                 << ((constraint->m_target[0] == 'L')
                     ? &(constraint->m_target[1]) : constraint->m_target)
                 << "\"");
        unsigned index = 0;
        while (constraint->m_target[index++] != 'L') {
            assert(constraint->m_target[index] != '\0');
        }
        VF_SET_CTX(ctx, "Couldn't load class: "
                   << &(constraint->m_target[index]));
        return VF_ErrorLoadClass;
    }
    // check if constraint is already verified
    if (VF_CHECK_NONE == constraint->m_check_type) {
        // already verified
        VF_TRACE("constraint.checked", "verify constraint: have \""
                 << constraint->m_source << "\" need \"" << constraint->
                 m_target << "\" already done (check #2) for class " <<
                 class_get_name(ctx->m_class));
        return VF_OK;
    }

    /**
     * Verifier which is built on Java VM Specification 2nd Edition (4.9.2)
     * recommendation of verification process doesn't check interfaces usage.
     * Unfortunately, a lot of Java applications depends on this neglect.
     * To be compatible with those applications we should do full constraint
     * checks only if -Xverify:all option is present in command line.
     */
    if (!ctx->m_verify_all && class_is_interface_(target)) {
        // skip constraint check
        // reset constraint to successful
        constraint->m_check_type = VF_CHECK_NONE;
        return VF_OK;
    }
    // check if constraint is already verified
    if (VF_CHECK_NONE == constraint->m_check_type) {
        // already verified
        VF_TRACE("constraint.checked", "verify constraint: have \""
                 << constraint->m_source << "\" need \"" << constraint->
                 m_target << "\" already done (check #3) for class " <<
                 class_get_name(ctx->m_class));
        return VF_OK;
    }
    // get stack reference class
    class_handler source = vf_resolve_class(constraint->m_source, true, ctx);
    if (!source) {
        VF_DEBUG("verifying class " << class_get_name(ctx->m_class)
                 << " (method " << constraint->m_name
                 << constraint->m_descriptor << ") couldn't load class \""
                 << ((constraint->m_source[0] == 'L')
                     ? &(constraint->m_source[1]) : constraint->m_source)
                 << "\"");
        unsigned index = 0;
        while (constraint->m_source[index++] != 'L') {
            assert(constraint->m_source[index] != '\0');
        }
        VF_SET_CTX(ctx, "Couldn't load class: "
                   << &(constraint->m_source[index]));
        return VF_ErrorLoadClass;
    }
    // store constraint check type (it could be changed during validation check)
    vf_CheckConstraint check = (vf_CheckConstraint) constraint->m_check_type;

    // check if constraint is already verified
    if (check == VF_CHECK_NONE) {
        // already verified
        VF_TRACE("constraint.checked", "verify constraint: have \""
                 << constraint->m_source << "\" need \"" << constraint->
                 m_target << "\" already done (check #4) for class " <<
                 class_get_name(ctx->m_class));
        return VF_OK;
    }
    // check restriction
    if (!vf_is_valid(source, target, ctx->m_class, check)) {
        // return error
        ctx->m_method = constraint->m_method;
        ctx->m_name = constraint->m_name;
        ctx->m_descriptor = constraint->m_descriptor;
        vf_set_error(check, ctx);
        return VF_ErrorIncompatibleArgument;
    }
    // reset constraint to successful
    constraint->m_check_type = VF_CHECK_NONE;
    return VF_OK;
}                               // vf_force_check_constraint

/**
 * Function verifies class constraints.
 */
static vf_Result vf_verify_class_constraints(vf_Context *ctx)   // verification context
{
    // get method verify data
    vf_TypeConstraint *constraint =
        (vf_TypeConstraint *) class_get_verify_data_ptr(ctx->m_class);
    if (constraint == NULL) {
        return VF_OK;
    }
    // trace verified class
    VF_TRACE("class.constraint",
             "verify constraints: " << class_get_name(ctx->m_class));

    // check method constraints
    vf_Result result = VF_OK;
    for (; constraint; constraint = constraint->m_next) {
        result = vf_force_check_constraint(constraint, ctx);
        if (VF_OK != result) {
            break;
        }
    }
    return result;
}                               // vf_verify_class_constraints

/************************************************************
 ******************** Memory functions **********************
 ************************************************************/

/**
 * Function allocates an array in memory with elements initialized to zero. 
 */
void *vf_calloc_func(unsigned num,      // number of elements
                     size_t size,       // size of element
                     VF_SOURCE_PARAMS)  // debug info
{
    assert(num);
    assert(size);
    void *result = STD_CALLOC(num, size);
    if (result == NULL) {
        // out of memory error
        VF_DIE("vf_calloc_func: out of memory");
    }
#if VF_TRACE_MEMORY
    // trace memory
    VF_TRACE("memory", VF_REPORT_SOURCE
             << "(calloc) allocate memory addr: " << result
             << ", size: " << size *
             num << " (" << num << " by " << size << ")");
#endif // VF_TRACE_MEMORY

    return result;
}                               // vf_calloc_func

/**
 * Function allocates memory blocks.
 */
void *vf_malloc_func(size_t size,       // size of memory block
                     VF_SOURCE_PARAMS)  // debug info
{
    assert(size);
    void *result = STD_MALLOC(size);
    if (result == NULL) {
        // out of memory error
        VF_DIE("vf_malloc_func: out of memory");
    }
#if VF_TRACE_MEMORY
    // trace memory
    VF_TRACE("memory", VF_REPORT_SOURCE
             << "(malloc) allocate memory addr: " << result
             << ", size: " << size);
#endif // VF_TRACE_MEMORY

    return result;
}                               // vf_malloc_func

/**
 * Function reallocates memory blocks.
 */
void *vf_realloc_func(void *pointer,    // old pointer
                      size_t size,      // size of memory block
                      VF_SOURCE_PARAMS) // debug info
{
    assert(size);
    void *result = STD_REALLOC(pointer, size);
    if (result == NULL) {
        // out of memory error
        VF_DIE("vf_realloc_func: out of memory");
    }
#if VF_TRACE_MEMORY
    // trace memory
    VF_TRACE("memory", VF_REPORT_SOURCE
             << "(realloc) reallocate memory from addr: " << pointer
             << " to addr: " << result << ", size: " << size);
#endif // VF_TRACE_MEMORY

    return result;
}                               // vf_realloc_func

/**
 * Function releases allocated memory blocks.
 */
void vf_free_func(void *pointer,        // free pointer
                  VF_SOURCE_PARAMS)     // debug info
{
    if (pointer) {
        STD_FREE(pointer);
    } else {
        VF_DIE("vf_free_func: null pointer for free");
    }

#if VF_TRACE_MEMORY
    // trace memory
    VF_TRACE("memory", VF_REPORT_SOURCE
             << "(free) free memory addr: " << (void *) pointer);
#endif // VF_TRACE_MEMORY

}                               // vf_free_func

/**
 * Function creates wide memory pool structure.
 */
static inline vf_PoolInternal *vf_create_pool_element(size_t size,      // initial pool size
                                                      VF_SOURCE_PARAMS) // debug info
{
    vf_PoolInternal *result;

    // create pool new entry and allocate memory for it
    result = (vf_PoolInternal *)
        vf_malloc_func(sizeof(vf_PoolInternal) + size, VF_SOURCE_ARGS1);
    result->m_memory = (char *) result + sizeof(vf_PoolInternal);
    result->m_free = (char *) result->m_memory;
    result->m_freesize = size;
    result->m_next = NULL;
    memset(result->m_memory, 0, size);

#if VF_TRACE_MEMORY
    // trace memory
    VF_TRACE("memory.pool.element", VF_REPORT_SOURCE
             << "(pool) create pool element: " << result
             << ", memory: " << result->
             m_memory << ", size: " << result->m_freesize);
#endif // VF_TRACE_MEMORY

    return result;
}                               // vf_create_pool_element

/**
 * Function creates memory pool structure.
 */
vf_Pool *vf_create_pool_func(VF_SOURCE_PARAMS)
{
    // create new pool element
    vf_PoolInternal *pool =
        vf_create_pool_element(sizeof(vf_Pool) + VF_POOL_ENTRY_SIZE,
                               VF_SOURCE_ARGS1);
    // set head pool
    vf_Pool *result = (vf_Pool *) pool->m_free;
    pool->m_memory = pool->m_free + sizeof(vf_Pool);
    pool->m_free = (char *) pool->m_memory;
    pool->m_freesize = VF_POOL_ENTRY_SIZE;
    result->m_pool = pool;
    result->m_memory = sizeof(vf_PoolInternal) + sizeof(vf_Pool)
        + VF_POOL_ENTRY_SIZE;
    result->m_used = 0;
    result->m_maxuse = 0;

#if VF_TRACE_MEMORY
    // trace memory
    VF_TRACE("memory.pool", VF_REPORT_SOURCE
             << "(pool) create pool: " << result);
#endif // VF_TRACE_MEMORY

    return result;
}                               // vf_create_pool_func

/**
 * Function allocates memory block in current pool.
 */
void *vf_palloc_func(vf_Pool *hpool,    // a given pool
                     size_t size,       // memory size
                     VF_SOURCE_PARAMS)  // debug info
{
    const unsigned align = sizeof(void *) - 1;
    void *result = NULL;
    vf_PoolInternal *pool = hpool->m_pool;

    // align allocate size
    size = (size + align) & (~align);
    // find free space
    if (size > VF_POOL_ENTRY_SIZE) {
        // create new wide pool entry
        pool = vf_create_pool_element(size, VF_SOURCE_ARGS1);
        pool->m_next = hpool->m_pool;
        hpool->m_pool = pool;
        hpool->m_memory += sizeof(vf_PoolInternal) + size;
    } else if (pool->m_freesize < size) {
        vf_PoolInternal *last = NULL;
        vf_PoolInternal *entry = pool->m_next;
        while (entry) {
            last = pool;
            pool = entry;
            if (pool->m_freesize >= size) {
                // found free space 
                break;
            }
            entry = pool->m_next;
        }
        if (!entry) {
            // create new pool element
            pool =
                vf_create_pool_element(VF_POOL_ENTRY_SIZE, VF_SOURCE_ARGS1);
            pool->m_next = hpool->m_pool;
            hpool->m_pool = pool;
            hpool->m_memory += sizeof(vf_PoolInternal) + VF_POOL_ENTRY_SIZE;
        } else {
            assert(last != NULL);
            last->m_next = pool->m_next;
            pool->m_next = hpool->m_pool;
            hpool->m_pool = pool;
        }
    }
    assert(hpool->m_used + size < hpool->m_memory);
    result = pool->m_free;
    pool->m_free += size;
    pool->m_freesize -= size;
    hpool->m_used += size;

#if VF_TRACE_MEMORY
    // trace memory
    VF_TRACE("memory.pool", VF_REPORT_SOURCE
             << "(pool) allocate memory in pool: " << hpool
             << ", memory: " << result << ", size: " << size
             << ", element: " << pool << ", free: " << pool->m_freesize);
#endif // VF_TRACE_MEMORY

    return result;
}                               // vf_palloc_func

/**
 * Function cleans given pool.
 */
void vf_clean_pool_func(vf_Pool *hpool, // memory pool
                        VF_SOURCE_PARAMS)       // debug info
{
    // set max used value
    if (hpool->m_used > hpool->m_maxuse) {
        hpool->m_maxuse = hpool->m_used;
    }
#if VF_TRACE_MEMORY
    // trace memory
    VF_TRACE("memory.pool", VF_REPORT_SOURCE
             << "(pool) clean pool: " << hpool
             << ", allocated: " << hpool->
             m_memory << ", used: " << hpool->m_used);
#endif // VF_TRACE_MEMORY

    vf_PoolInternal *pool = hpool->m_pool;
    while (pool) {
        // clean pool element space
        unsigned used_size =
            (unsigned) (pool->m_free - (char *) pool->m_memory);
        memset(pool->m_memory, 0, used_size);
        pool->m_free = (char *) pool->m_memory;
        pool->m_freesize += used_size;
        hpool->m_used -= used_size;

#if VF_TRACE_MEMORY
        // trace memory
        VF_TRACE("memory.pool.element", VF_REPORT_SOURCE
                 << "(pool) clean pool element: " << pool
                 << ", size: " << used_size);
#endif // VF_TRACE_MEMORY

        // get next pool entry
        pool = pool->m_next;
    }
}                               // vf_clean_pool_func

/**
 * Function releases memory from given pool.
 */
void vf_delete_pool_func(vf_Pool *hpool,        // memory pool
                         VF_SOURCE_PARAMS)      // debug info
{
#if VF_TRACE_MEMORY
    // trace memory
    VF_TRACE("memory.pool", VF_REPORT_SOURCE
             << "(pool) delete pool: " << hpool
             << ", allocated: " << hpool->m_memory
             << ", used: "
             << (hpool->m_used >
                 hpool->m_maxuse ? hpool->m_used : hpool->m_maxuse));
#endif // VF_TRACE_MEMORY

    vf_PoolInternal *pool = hpool->m_pool;
    while (pool) {
#if VF_TRACE_MEMORY
        // trace memory
        VF_TRACE("memory.pool.element", VF_REPORT_SOURCE
                 << "(pool) delete pool element: " << pool);
#endif // VF_TRACE_MEMORY

        // store pool element
        vf_PoolInternal *entry = pool;
        // get next pool element
        pool = pool->m_next;
        // free pool element
        vf_free_func(entry, VF_SOURCE_ARGS1);
    }
}                               // vf_delete_pool_func

/**
 * Provides final constraint checks for a given class.
 */
vf_Result vf_verify_class_constraints(class_handler klass,      // a given class
                                      unsigned verifyAll,       // verification level flag
                                      char **message)   // verifier error message
{
    assert(klass);
    assert(message);

    // create context
    vf_Context context;
    context.m_class = klass;
    context.m_verify_all = verifyAll ? true : false;

    // verified constraint for a given method
    vf_Result result = vf_verify_class_constraints(&context);
    *message = context.m_error;

#if _VF_DEBUG
    if (VF_OK != result) {
        VF_DEBUG("VerifyError: " << context.m_error);
    }
#endif // _VF_DEBUG

    return result;
}                               // vf_verify_class_constraints

/**
 * Function releases verify data in class loader.
 */
void vf_release_verify_data(void *data)
{
    vf_ClassLoaderData *cl_data = (vf_ClassLoaderData *) data;

    delete cl_data->string;
    vf_delete_pool(cl_data->pool);
}                               // vf_release_verify_data
