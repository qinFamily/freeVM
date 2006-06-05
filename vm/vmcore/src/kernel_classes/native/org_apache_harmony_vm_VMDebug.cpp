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
 * @version $Revision: 1.1.6.5 $
 */  

/**
 * @file org_apache_harmony_vm_VMDebug.cpp
 *
 * This file is a part of kernel class natives VM core component.
 * It contains implementation for native methods of
 * org.apache.harmony.drl.vm.VMDebug class.
 */

#include "org_apache_harmony_vm_VMDebug.h"

/**
 * Implements org.apache.harmony.drl.vm.VMDebug.print(..) method.
 * Provides debug printing. This method may be used for development process
 * only, it is not required for a work of release version of VM. Allows 
 * printing messages to standard output before the printing can be done
 * through java.lang.System.out/err channel. 
 */
JNIEXPORT void JNICALL Java_org_apache_harmony_vm_VMDebug_print
  (JNIEnv *jenv, jclass, jstring message)
{
    const char* n= jenv->GetStringUTFChars(message, NULL);
    fprintf(stderr, "%s", n); fflush(stderr);
    jenv->ReleaseStringUTFChars(message, n);
}
