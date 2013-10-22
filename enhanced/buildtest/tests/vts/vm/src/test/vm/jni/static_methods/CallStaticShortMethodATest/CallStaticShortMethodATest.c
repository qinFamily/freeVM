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

#define ARG1 ((jshort)100)
#define RES1 ((jshort)ARG1)
#define ARG22 ((jshort)1500)
#define RES2 ((jshort)(ARG1 + ARG22))
#define ARG3_LENGTH 2
#define ARG31 ((jshort)-32755)
#define ARG32 ((jshort)13211)
#define RES3 ((jshort)(ARG31 + ARG32))
#define ARG42_LENGTH 3
#define ARG421 ((jshort)881)
#define ARG422 ((jshort)-1)
#define ARG423 ((jshort)32756)
#define RES4 ((jshort)((ARG31 + ARG32) + (ARG421 + ARG422 + ARG423)))
#define ARG5 ((jshort)10021)
#define RES5 ((jshort)ARG5)
#define ARG62 ((jshort)4800)
#define RES6 ((jshort)(ARG5 + ARG62))

/*
 * Method: org.apache.harmony.vts.test.vm.jni.static_methods.CallStaticShortMethodATest.nativeExecute(Lorg/apache/harmony/vts/test/vm/jni/static_methods/TestClass;Lorg/apache/harmony/vts/test/vm/jni/static_methods/NativeTestClass;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_vts_test_vm_jni_static_1methods_CallStaticShortMethodATest_nativeExecute
    (JNIEnv *env, jobject this_object, jobject tc, jobject ntc)
{
    jclass sc, class1, class2;
    char *sig1 = "(S)S";
    char *sig2 = "(SS)S";
    char *sig3 = "([S)S";
    char *sig4 = "([S[S)S";
    char *sig5 = "(Ljava/lang/Short;)S";
    char *sig6 = "(Ljava/lang/Short;Ljava/lang/Short;)S";
    jobject so5, so62;
    jshortArray ar3, ar42;
    jshort *elements3, *elements42;
    jshort result1, result2, result3, result4, result5, result6;
    jshort nresult1, nresult2, nresult3, nresult4, nresult5, nresult6;
    jvalue args1[1], args2[2], args3[1], args4[2], args5[1], args6[2];

    jmethodID m1, m2, m3, m4, m5, m6;
    jmethodID nm1, nm2, nm3, nm4, nm5, nm6;
    jmethodID short_init;

    ar3 = (*env)->NewShortArray(env, ARG3_LENGTH);
    ar42 = (*env)->NewShortArray(env, ARG42_LENGTH);

    if (NULL == ar3 || NULL == ar42)
        return JNI_FALSE;

    elements3 = (*env)->GetShortArrayElements(env, ar3, NULL);
    elements42 = (*env)->GetShortArrayElements(env, ar42, NULL);

    if (NULL == elements3 || NULL == elements42)
        return JNI_FALSE;

    elements3[0] = ARG31;
    elements3[1] = ARG32;

    elements42[0] = ARG421;
    elements42[1] = ARG422;
    elements42[2] = ARG423;

    (*env)->ReleaseShortArrayElements(env, ar3, elements3, 0);
    (*env)->ReleaseShortArrayElements(env, ar42, elements42, 0);

    sc = (*env)->FindClass(env, "java/lang/Short");
    if (NULL == sc)
        return JNI_FALSE;
    short_init = (*env)->GetMethodID(env, sc, "<init>", "(S)V");
    if (NULL == short_init)
        return JNI_FALSE;
    so5 = (*env)->NewObject(env, sc, short_init, ARG5);
    so62 = (*env)->NewObject(env, sc, short_init, ARG62);
    if (NULL == so5 || NULL == so62)
        return JNI_FALSE;

    class1 = (*env)->GetObjectClass(env, tc);
    class2 = (*env)->GetObjectClass(env, ntc);

    m1 = (*env)->GetStaticMethodID(env, class1, "method", sig1);
    m2 = (*env)->GetStaticMethodID(env, class1, "method", sig2);
    m3 = (*env)->GetStaticMethodID(env, class1, "method", sig3);
    m4 = (*env)->GetStaticMethodID(env, class1, "method", sig4);
    m5 = (*env)->GetStaticMethodID(env, class1, "method", sig5);
    m6 = (*env)->GetStaticMethodID(env, class1, "method", sig6);

    if (NULL == m1 || NULL == m2 || NULL == m3 ||
        NULL == m4 || NULL == m5 || NULL == m6)
        return JNI_FALSE;

    nm1 = (*env)->GetStaticMethodID(env, class2, "method", sig1);
    nm2 = (*env)->GetStaticMethodID(env, class2, "method", sig2);
    nm3 = (*env)->GetStaticMethodID(env, class2, "method", sig3);
    nm4 = (*env)->GetStaticMethodID(env, class2, "method", sig4);
    nm5 = (*env)->GetStaticMethodID(env, class2, "method", sig5);
    nm6 = (*env)->GetStaticMethodID(env, class2, "method", sig6);

    if (NULL == nm1 || NULL == nm2 || NULL == nm3 ||
        NULL == nm4 || NULL == nm5 || NULL == nm6)
        return JNI_FALSE;

    args1[0].s = ARG1;
    args2[0].s = ARG1;
    args2[1].s = ARG22;
    args3[0].l = ar3;
    args4[0].l = ar3;
    args4[1].l = ar42;
    args5[0].l = so5;
    args6[0].l = so5;
    args6[1].l = so62;

    result1 = (*env)->CallStaticShortMethodA(env, class1, m1, args1);
    result2 = (*env)->CallStaticShortMethodA(env, class1, m2, args2);
    result3 = (*env)->CallStaticShortMethodA(env, class1, m3, args3);
    result4 = (*env)->CallStaticShortMethodA(env, class1, m4, args4);
    result5 = (*env)->CallStaticShortMethodA(env, class1, m5, args5);
    result6 = (*env)->CallStaticShortMethodA(env, class1, m6, args6);

    nresult1 = (*env)->CallStaticShortMethodA(env, class2, nm1, args1);
    nresult2 = (*env)->CallStaticShortMethodA(env, class2, nm2, args2);
    nresult3 = (*env)->CallStaticShortMethodA(env, class2, nm3, args3);
    nresult4 = (*env)->CallStaticShortMethodA(env, class2, nm4, args4);
    nresult5 = (*env)->CallStaticShortMethodA(env, class2, nm5, args5);
    nresult6 = (*env)->CallStaticShortMethodA(env, class2, nm6, args6);

    if (result1 != RES1 || nresult1 != RES1 ||
        result2 != RES2 || nresult2 != RES2 ||
        result3 != RES3 || nresult3 != RES3 ||
        result4 != RES4 || nresult4 != RES4 ||
        result5 != RES5 || nresult5 != RES5 ||
        result6 != RES6 || nresult6 != RES6)
        return JNI_FALSE;
    else
        return JNI_TRUE;
}
