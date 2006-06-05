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
 * @author Euguene Ostrovsky
 * @version $Revision: 1.1.6.6 $
 */  

/*
 * THE FILE HAS BEEN AUTOGENERATED BY INTEL IJH TOOL.
 * Please be aware that all changes made to this file manually
 * will be overwritten by the tool if it runs again.
 */

#include <jni.h>


/* Header for class org.apache.harmony.vm.VMStack */

#ifndef _ORG_APACHE_HARMONY_VM_VMSTACK_H
#define _ORG_APACHE_HARMONY_VM_VMSTACK_H

#ifdef __cplusplus
extern "C" {
#endif


/* Native methods */

/*
 * Method: org.apache.harmony.vm.VMStack.getCallerClass(I)Ljava/lang/Class;
 */
JNIEXPORT jclass JNICALL
Java_org_apache_harmony_vm_VMStack_getCallerClass(JNIEnv *, jclass, 
    jint);

/*
 * Method: org.apache.harmony.vm.VMStack.getClasses(IZ)[Ljava/lang/Class;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_apache_harmony_vm_VMStack_getClasses(JNIEnv *, jclass, 
    jint, jboolean);

/*
 * Method: org.apache.harmony.vm.VMStack.getStackState()Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_apache_harmony_vm_VMStack_getStackState(JNIEnv *, jclass);

/*
 * Method: org.apache.harmony.vm.VMStack.getStackTrace(Ljava/lang/Object;)[Ljava/lang/StackTraceElement;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_apache_harmony_vm_VMStack_getStackTrace(JNIEnv *, jclass, 
    jobject);

/*
 * Method: org.apache.harmony.vm.VMStack.getClassLoader(Ljava/lang/Class;)Ljava/lang/ClassLoader;
 */
JNIEXPORT jobject JNICALL
Java_org_apache_harmony_vm_VMStack_getClassLoader(JNIEnv *, jclass, 
    jclass);


#ifdef __cplusplus
}
#endif

#endif /* _ORG_APACHE_HARMONY_VM_VMSTACK_H */

