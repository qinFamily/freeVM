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
 * @author Alexey A. Petrenko
 * @version $Revision$
 */
/*
 * THE FILE HAS BEEN AUTOGENERATED BY INTEL IJH TOOL.
 * Please be aware that all changes made to this file manually
 * will be overwritten by the tool if it runs again.
 */

#include <jni.h>


/* Header for class org.apache.harmony.awt.theme.windows.WinThemeGraphics */

#ifndef _ORG_APACHE_HARMONY_AWT_THEME_WINDOWS_WINTHEMEGRAPHICS_H
#define _ORG_APACHE_HARMONY_AWT_THEME_WINDOWS_WINTHEMEGRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif


/* Native methods */

/*
 * Method: org.apache.harmony.awt.theme.windows.WinThemeGraphics.fillHatchedSysColorRect(JIIIIII)V
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_awt_theme_windows_WinThemeGraphics_fillHatchedSysColorRect(JNIEnv *, jclass, 
    jlong, jint, jint, jint, jint, jint, jint);

/*
 * Method: org.apache.harmony.awt.theme.windows.WinThemeGraphics.drawEdge(JIIIII)V
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_awt_theme_windows_WinThemeGraphics_drawEdge(JNIEnv *, jclass, 
    jlong, jint, jint, jint, jint, jint);

/*
 * Method: org.apache.harmony.awt.theme.windows.WinThemeGraphics.drawFocusRect(JIIII)V
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_awt_theme_windows_WinThemeGraphics_drawFocusRect(JNIEnv *, jclass, 
    jlong, jint, jint, jint, jint);

/*
 * Method: org.apache.harmony.awt.theme.windows.WinThemeGraphics.fillBackground(JIIIIIZ)V
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_awt_theme_windows_WinThemeGraphics_fillBackground(JNIEnv *, jclass, 
    jlong, jint, jint, jint, jint, jint, jboolean);

/*
 * Method: org.apache.harmony.awt.theme.windows.WinThemeGraphics.drawClassicBackground(JIIIIII)V
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_awt_theme_windows_WinThemeGraphics_drawClassicBackground(JNIEnv *, jclass, 
    jlong, jint, jint, jint, jint, jint, jint);

/*
 * Method: org.apache.harmony.awt.theme.windows.WinThemeGraphics.drawXpBackground(JIIIIJII)V
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_awt_theme_windows_WinThemeGraphics_drawXpBackground(JNIEnv *, jclass, 
    jlong, jint, jint, jint, jint, jlong, jint, jint);

/*
 * Method: org.apache.harmony.awt.theme.windows.WinThemeGraphics.restoreGdiClip(JJ)V
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_awt_theme_windows_WinThemeGraphics_restoreGdiClip(JNIEnv *, jclass, 
    jlong, jlong);

/*
 * Method: org.apache.harmony.awt.theme.windows.WinThemeGraphics.setGdiClip(J[II)J
 */
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_awt_theme_windows_WinThemeGraphics_setGdiClip(JNIEnv *, jclass, 
    jlong, jintArray, jint);


#ifdef __cplusplus
}
#endif

#endif /* _ORG_APACHE_HARMONY_AWT_THEME_WINDOWS_WINTHEMEGRAPHICS_H */

