/*
 *  Copyright 2005 - 2006 The Apache Software Foundation or its licensors, as applicable.
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
 * @author Pavel Dolgov
 * @version $Revision$
 */
#include <jni.h>

#ifndef _Included_org_apache_harmony_awt_nativebridge_windows_WinManagement
#define _Included_org_apache_harmony_awt_nativebridge_windows_WinManagement
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jintArray JNICALL Java_org_apache_harmony_awt_nativebridge_windows_WinManagement_getObscuredRegionImpl
  (JNIEnv * env, jclass clazz, jlong hwnd, jint x, jint y, jint w, jint h);

#ifdef __cplusplus
}
#endif
#endif
