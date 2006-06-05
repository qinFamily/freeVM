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
 * @version $Revision: 1.1.2.1.4.6 $
 */  

/*
 * THE FILE HAS BEEN AUTOGENERATED BY INTEL IJH TOOL.
 * Please be aware that all changes made to this file manually
 * will be overwritten by the tool if it runs again.
 */

#include <jni.h>


/* Header for class java.lang.Class */

#ifndef _JAVA_LANG_CLASS_H
#define _JAVA_LANG_CLASS_H

#ifdef __cplusplus
extern "C" {
#endif


/* Static final fields */

#undef java_lang_Class_serialVersionUID
#define java_lang_Class_serialVersionUID 3206093459760846163LL


/* Native methods */

/*
 * Method: java.lang.Class.getStackClasses(IZ)[Ljava/lang/Class;
 */
JNIEXPORT jobjectArray JNICALL
Java_java_lang_Class_getStackClasses(JNIEnv *, jclass, 
    jint, jboolean);


#ifdef __cplusplus
}
#endif

#endif /* _JAVA_LANG_CLASS_H */

