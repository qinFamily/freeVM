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
package org.apache.harmony.vts.test.vm.jvmti;

/** 
 * @author Valentin Al. Sitnick
 * @version $Revision: 1.1 $
 *
 */ 
public class GetClassLoader0102 {

    static public void main(String args[]) {

        CustomClassLoader_GetClassLoader0102 ccl = new CustomClassLoader_GetClassLoader0102();

        try {
            Class specclass = ccl.loadClass("org.apache.harmony.vts.test.vm.jvmti.SpecialClass002");
            SpecialClass002 MyClass = (SpecialClass002) specclass.newInstance();
            MyClass.superMegaMethod();
            MyClass = null;
            specclass = null;
            ccl = null;
        } catch (Throwable tex) { }
        return;
    }
}

class CustomClassLoader_GetClassLoader0102 extends ClassLoader {
    int fake = 0;
}

