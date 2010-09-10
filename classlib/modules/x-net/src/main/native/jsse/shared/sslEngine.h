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
#include <jni.h>

#ifndef _SSLENGINE_H
#define _SSLENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_org_apache_harmony_xnet_provider_jsse_SSLEngineImpl_initImpl
  (JNIEnv *, jclass, jlong);
JNIEXPORT jobject JNICALL Java_org_apache_harmony_xnet_provider_jsse_SSLEngineImpl_acceptImpl
  (JNIEnv *, jclass, jlong);
JNIEXPORT jobject JNICALL Java_org_apache_harmony_xnet_provider_jsse_SSLEngineImpl_connectImpl
  (JNIEnv *, jclass, jlong);
JNIEXPORT jobject JNICALL Java_org_apache_harmony_xnet_provider_jsse_SSLEngineImpl_wrapImpl
  (JNIEnv *, jclass, jlong, jbyteArray, int, jbyteArray, int);
#ifdef __cplusplus
}
#endif

#endif
