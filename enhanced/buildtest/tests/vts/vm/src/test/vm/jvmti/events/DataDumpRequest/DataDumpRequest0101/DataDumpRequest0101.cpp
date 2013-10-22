/*
    Copyright 2005-2006 The Apache Software Foundation or its licensors, as applicable

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

    See the License for the specific language governing permissions and
    limitations under the License.
*/
/**
 * @author Valentin Al. Sitnick
 * @version $Revision: 1.2 $
 *
 */

/* *********************************************************************** */

#include <signal.h>

#include "events.h"
#include "utils.h"
#include "fake.h"

static bool params = false;
static bool test = false;
static bool flag = false;
static long counter = 0;

/* *********************************************************************** */

/**
 * test of event DataDumpRequest
 */
void DataDumpRequest0101()
{
    //Fake method for docletting only
}

/* *********************************************************************** */

JNIEXPORT jint JNICALL Agent_OnLoad(prms_AGENT_ONLOAD)
{
    check_AGENT_ONLOAD;

    Callbacks CB;

    cb_exc;
    cb_dump;
    cb_death;

    AGENT_FOR_EVENTS_TESTS_PART_I; /* events.h */

    jvmtiEvent events[] = { JVMTI_EVENT_EXCEPTION, JVMTI_EVENT_DATA_DUMP_REQUEST,
        JVMTI_EVENT_VM_DEATH };

    AGENT_FOR_EVENTS_TESTS_PART_II;

    fprintf(stderr, "\n-------------------------------------------------\n");
    fprintf(stderr, "\ntest DataDumpRequest0101 is started\n{\n");
    fflush(stderr);

    return JNI_OK;
}

/* *********************************************************************** */

void JNICALL callbackException(prms_EXCPT)
{
    check_EXCPT;

    if (flag) return;

    char* name;
    char* signature;
    char* generic;
    jvmtiError result;
    jvmtiPhase phase;

    fprintf(stderr, "\tnative: Exception (prms) jvmti_env is %p\n", jvmti_env);
    fflush(stderr);
    result = jvmti_env->GetPhase(&phase);
    fprintf(stderr, "\tnative: GetPhase result = %d (must be zero) \n", result);
    fprintf(stderr, "\tnative: current phase is %d (must be 4 LIVE-phase) \n", phase);
    if (result != JVMTI_ERROR_NONE) return;

    if (phase != JVMTI_PHASE_LIVE)
    {
        fprintf(stderr, "\tnative: INCORRECT phase for this event. Test failed\n");
        flag = true;
        return;
    }

    result = jvmti_env->GetMethodName(method, &name, &signature, &generic);
    fprintf(stderr, "\tnative: GetMethodName result = %d (must be zero) \n", result);
    fprintf(stderr, "\tnative: method name is %s \n", name);
    fprintf(stderr, "\tnative: signature name is %s \n", signature);
    fprintf(stderr, "\tnative: generic name is %s \n", generic);
    fflush(stderr);

    if (result != JVMTI_ERROR_NONE) return;
    if (strcmp(name, "special_method")) return;

#ifdef LINUX

    fprintf(stderr, "\tnative: kill SIGINT is %d \n", SIGINT);
    fprintf(stderr, "\tnative: kill getpid() is %d \n", getpid());
    fflush(stderr);

    usleep(50000);

    for (int i = 1 ; i <= 15; i++)
        kill(i, getpid());
#else

    //TODO


#endif

    //this check must be fixed
    params = true;
    test = true;
}

void JNICALL callbackDataDumpRequest(prms_DATA_DUMP)
{
    flag = true;

    check_DATA_DUMP;

    jvmtiError result;
    jvmtiPhase phase;
    counter++;

    fprintf(stderr, "\tnative: DataDumpRequest (prms) jvmti_env is %p\n", jvmti_env);
    fflush(stderr);

    result = jvmti_env->GetPhase(&phase);

    fprintf(stderr, "\tnative: GetPhase result = %d (must be zero) \n", result);
    fprintf(stderr, "\tnative: current phase is %d (must be 4 LIVE-phase) \n", phase);

    if (result != JVMTI_ERROR_NONE) return;

    if (phase != JVMTI_PHASE_LIVE)
    {
        fprintf(stderr, "\tnative: INCORRECT phase for this event. Test failed\n");
        flag = true;
        return;
    }

    //this check must be fixed
    params = true;
    test = true;
}

void JNICALL callbackVMDeath(prms_VMDEATH)
{
    check_VMDEATH;

    fprintf(stderr, "\n\tTest of event DataDumpRequest0101                : ");

    if ((counter == 1) && (params) && (test))
        fprintf(stderr, " passed \n");
    else
        fprintf(stderr, " failed \n");

    fprintf(stderr, "\n} /* test DataDumpRequest0101 is finished */ \n");
    fflush(stderr);
}

/* *********************************************************************** */




