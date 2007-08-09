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
/** 
 * @author Alexey A. Petrenko 
 * @version $Revision: 1.3 $ 
 */ 

#include <jni.h>


/* Header for class org.apache.harmony.x.print.PSInterpreter */

#ifndef _ORG_APACHE_HARMONY_X_PRINT_PSINTERPRETER_H
#define _ORG_APACHE_HARMONY_X_PRINT_PSINTERPRETER_H

#ifdef __cplusplus
extern "C" {
#endif


/* Static final fields */

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_MOVETO
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_MOVETO 0L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_LINETO
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_LINETO 1L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_SETRGBCOLOR
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_SETRGBCOLOR 2L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_STROKE
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_STROKE 3L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_COLORIMAGE
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_COLORIMAGE 4L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_TRANSLATE
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_TRANSLATE 5L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_SCALE
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_SCALE 6L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_ARC
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_ARC 7L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_SHOW
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_SHOW 8L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_NEWPATH
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_NEWPATH 9L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_CLOSEPATH
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_CLOSEPATH 10L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_FILL
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_FILL 11L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_FINDFONT
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_FINDFONT 12L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_SCALEFONT
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_SCALEFONT 13L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_SETFONT
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_SETFONT 14L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_ROTATE
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_ROTATE 15L

#undef org_apache_harmony_x_print_PSInterpreter_COMMAND_CLIP
#define org_apache_harmony_x_print_PSInterpreter_COMMAND_CLIP 16L


/* Native methods */

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.obtainServiceID(Ljava/lang/String;II)I
 */
JNIEXPORT jint JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_obtainServiceID(JNIEnv *, jclass, 
    jstring, jint, jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.releaseServiceID(I)V
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_releaseServiceID(JNIEnv *, jclass, 
    jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.startDocument(Ljava/lang/String;I[ILjava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_startDocument(JNIEnv *, jclass, 
    jstring, jint, jintArray, jstring, jstring);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.startPage(I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_startPage(JNIEnv *, jclass, 
    jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.endPage(I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_endPage(JNIEnv *, jclass, 
    jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.endDocument(I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_endDocument(JNIEnv *, jclass, 
    jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.setRGBColor(IDDD)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_setRGBColor(JNIEnv *, jclass, 
    jint, jdouble, jdouble, jdouble);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.moveTo(IDD)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_moveTo(JNIEnv *, jclass, 
    jint, jdouble, jdouble);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.lineTo(IDD)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_lineTo(JNIEnv *, jclass, 
    jint, jdouble, jdouble);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.drawArc(IDDDDD)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_drawArc(JNIEnv *, jclass, 
    jint, jdouble, jdouble, jdouble, jdouble, jdouble);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.drawText(ILjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_drawText(JNIEnv *, jclass, 
    jint, jstring);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.drawImage(IDDDD[III)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_drawImage(JNIEnv *, jclass, 
    jint, jdouble, jdouble, jdouble, jdouble, jintArray, jint, jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.beginPath(I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_beginPath(JNIEnv *, jclass, 
    jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.closePath(I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_closePath(JNIEnv *, jclass, 
    jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.strokePath(I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_strokePath(JNIEnv *, jclass, 
    jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.fillPath(I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_fillPath(JNIEnv *, jclass, 
    jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.clipPath(I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_clipPath(JNIEnv *, jclass, 
    jint);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.rotate(ID)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_rotate(JNIEnv *, jclass, 
    jint, jdouble);

/*
 * Method: org.apache.harmony.x.print.PSInterpreter.setFont(ILjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_x_print_PSInterpreter_setFont(JNIEnv *, jclass, 
    jint, jstring);


#ifdef __cplusplus
}
#endif

#endif /* _ORG_APACHE_HARMONY_X_PRINT_PSINTERPRETER_H */

