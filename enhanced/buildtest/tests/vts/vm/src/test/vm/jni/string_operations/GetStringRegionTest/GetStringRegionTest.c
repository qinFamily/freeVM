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
  
#include <jni.h>

/*
 * Method: org.apache.harmony.vts.test.vm.jni.string_operations.GetStringRegionTest.nativeExecute(Ljava/lang/String;II)[C
 */
JNIEXPORT jcharArray JNICALL
Java_org_apache_harmony_vts_test_vm_jni_string_1operations_GetStringRegionTest_nativeExecute
    (JNIEnv *env, jobject this_object, jstring str, jint start, jint length)
{
    jcharArray result;
    jchar *elements;

    result = (*env)->NewCharArray(env, length);
    if (NULL == result)
        return NULL;

    elements = (*env)->GetCharArrayElements(env, result, NULL);
    if (NULL == elements)
        return NULL;

    (*env)->GetStringRegion(env, str, start, length, elements);

    (*env)->ReleaseCharArrayElements(env, result, elements, 0);

    return result;
}
