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

/*
 * THE FILE HAS BEEN AUTOGENERATED BY INTEL IJH TOOL.
 * Please be aware that all changes made to this file manually
 * will be overwritten by the tool if it runs again.
 */

#include <jni.h>


/* Header for class java.lang.VMClassRegistry */

#ifndef _JAVA_LANG_VMCLASSREGISTRY_H
#define _JAVA_LANG_VMCLASSREGISTRY_H

#ifdef __cplusplus
extern "C" {
#endif


/* Native methods */

/*
 * Method: java.lang.VMClassRegistry.loadBootstrapClass(Ljava/lang/String;)Ljava/lang/Class;
 * Throws: java.lang.NoClassDefFoundError
 */
JNIEXPORT jclass JNICALL
Java_java_lang_VMClassRegistry_loadBootstrapClass(JNIEnv *, jclass, 
    jstring);

/*
 * Method: java.lang.VMClassRegistry.getClass(Ljava/lang/Object;)Ljava/lang/Class;
 */
JNIEXPORT jclass JNICALL
Java_java_lang_VMClassRegistry_getClass(JNIEnv *, jclass, 
    jobject);

/*
 * Method: java.lang.VMClassRegistry.getClassLoader(Ljava/lang/Class;)Ljava/lang/ClassLoader;
 */
JNIEXPORT jobject JNICALL
Java_java_lang_VMClassRegistry_getClassLoader0(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getComponentType(Ljava/lang/Class;)Ljava/lang/Class;
 */
JNIEXPORT jclass JNICALL
Java_java_lang_VMClassRegistry_getComponentType(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getDeclaredClasses(Ljava/lang/Class;)[Ljava/lang/Class;
 */
JNIEXPORT jobjectArray JNICALL
Java_java_lang_VMClassRegistry_getDeclaredClasses(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getDeclaredConstructors(Ljava/lang/Class;)[Ljava/lang/reflect/Constructor;
 */
JNIEXPORT jobjectArray JNICALL
Java_java_lang_VMClassRegistry_getDeclaredConstructors(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getDeclaredFields(Ljava/lang/Class;)[Ljava/lang/reflect/Field;
 */
JNIEXPORT jobjectArray JNICALL
Java_java_lang_VMClassRegistry_getDeclaredFields(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getDeclaredMethods(Ljava/lang/Class;)[Ljava/lang/reflect/Method;
 */
JNIEXPORT jobjectArray JNICALL
Java_java_lang_VMClassRegistry_getDeclaredMethods(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getDeclaringClass(Ljava/lang/Class;)Ljava/lang/Class;
 */
JNIEXPORT jclass JNICALL
Java_java_lang_VMClassRegistry_getDeclaringClass(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getInterfaces(Ljava/lang/Class;)[Ljava/lang/Class;
 */
JNIEXPORT jobjectArray JNICALL
Java_java_lang_VMClassRegistry_getInterfaces(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getModifiers(Ljava/lang/Class;)I
 */
JNIEXPORT jint JNICALL
Java_java_lang_VMClassRegistry_getModifiers(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getName(Ljava/lang/Class;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_java_lang_VMClassRegistry_getName(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getSuperclass(Ljava/lang/Class;)Ljava/lang/Class;
 */
JNIEXPORT jclass JNICALL
Java_java_lang_VMClassRegistry_getSuperclass(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.getSystemPackages(I)[[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL
Java_java_lang_VMClassRegistry_getSystemPackages(JNIEnv *, jclass, 
    jint);

/*
 * Method: java.lang.VMClassRegistry.initializeClass(Ljava/lang/Class;)V
 */
JNIEXPORT void JNICALL
Java_java_lang_VMClassRegistry_initializeClass(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.isArray(Ljava/lang/Class;)Z
 */
JNIEXPORT jboolean JNICALL
Java_java_lang_VMClassRegistry_isArray(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.isAssignableFrom(Ljava/lang/Class;Ljava/lang/Class;)Z
 */
JNIEXPORT jboolean JNICALL
Java_java_lang_VMClassRegistry_isAssignableFrom(JNIEnv *, jclass, 
    jclass, jclass);

/*
 * Method: java.lang.VMClassRegistry.isInstance(Ljava/lang/Class;Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL
Java_java_lang_VMClassRegistry_isInstance(JNIEnv *, jclass, 
    jclass, jobject);

/*
 * Method: java.lang.VMClassRegistry.isPrimitive(Ljava/lang/Class;)Z
 */
JNIEXPORT jboolean JNICALL
Java_java_lang_VMClassRegistry_isPrimitive(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.linkClass(Ljava/lang/Class;)V
 */
JNIEXPORT void JNICALL
Java_java_lang_VMClassRegistry_linkClass(JNIEnv *, jclass, 
    jclass);

/*
 * Method: java.lang.VMClassRegistry.loadLibrary(Ljava/lang/String;Ljava/lang/ClassLoader;)V
 */
JNIEXPORT void JNICALL
Java_java_lang_VMClassRegistry_loadLibrary(JNIEnv *, jclass, 
    jstring, jobject);

/*
* Method: java.lang.VMClassRegistry.getEnclosingClass(Ljava/lang/Class;)Ljava/lang/Class;
*/
JNIEXPORT jclass JNICALL Java_java_lang_VMClassRegistry_getEnclosingClass
(JNIEnv *, jclass, jclass);

/*
* Method: java.lang.VMClassRegistry.getEnclosingMember(Ljava/lang/Class;)Ljava/lang/reflect/Member;
*/
JNIEXPORT jobject JNICALL Java_java_lang_VMClassRegistry_getEnclosingMember
(JNIEnv *, jclass, jclass);

/*
* Method: java.lang.VMClassRegistry.getSimpleName(Ljava/lang/Class;)Ljava/lang/String;
*/
JNIEXPORT jstring JNICALL Java_java_lang_VMClassRegistry_getSimpleName
(JNIEnv *, jclass, jclass);

#ifdef __cplusplus
}
#endif

#endif /* _JAVA_LANG_VMCLASSREGISTRY_H */

