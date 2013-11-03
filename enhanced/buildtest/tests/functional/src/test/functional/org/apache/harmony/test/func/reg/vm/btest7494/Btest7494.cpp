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
 */

#include <stdio.h>
#include <string.h>
#include "jvmti.h"

static bool status = false;

JNIEXPORT void JNICALL VMDeath(jvmtiEnv *jvmti_env, JNIEnv *env)
{
    fprintf(stderr, status ? " passed \n" : " failed \n");
}

JNIEXPORT void JNICALL ThreadStart(jvmtiEnv *jvmti_env,
    JNIEnv *jni_env,
    jthread thread)
{
    jvmtiError err;
    char *err_str;

    jvmtiFrameInfo frames[5];
    jint count;

    jvmtiPhase phase;

    err = jvmti_env->GetPhase(&phase);
    if (JVMTI_ERROR_NONE != err)
    {
        jvmti_env->GetErrorName(err, &err_str);
        fprintf(stderr, "GetPhase returned %s\n", err_str);
        return;
    }

    if (JVMTI_PHASE_LIVE != phase)
        return;

    err = jvmti_env->GetStackTrace(NULL, 0, 5, frames, &count);
    if (JVMTI_ERROR_NONE != err)
    {
        jvmti_env->GetErrorName(err, &err_str);
        fprintf(stderr, "GetStackTrace returned %s\n", err_str);
    }

    status = true;
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM * jvm, char *options, void *reserved)
{
    jint res;

    jvmtiEnv *jvmti_env;
    res = jvm->GetEnv((void **)&jvmti_env, JVMTI_VERSION_1_0);

    if (res != JNI_OK || jvmti_env == NULL)
        return JNI_ERR;

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    callbacks.VMDeath = VMDeath;
    callbacks.ThreadStart = ThreadStart;

    jvmti_env->SetEventCallbacks(&callbacks, sizeof(callbacks));
    jvmti_env->SetEventNotificationMode(JVMTI_ENABLE,
        JVMTI_EVENT_VM_DEATH, NULL);
    jvmti_env->SetEventNotificationMode(JVMTI_ENABLE,
         JVMTI_EVENT_THREAD_START, NULL);

    return JNI_OK;
}
