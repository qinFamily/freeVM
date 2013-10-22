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
 * @author Gregory Shimansky, Petr Ivanov
 * @version $Revision: 1.3 $
 */  
/*
 * Created on 15.11.2004
 */
package org.apache.harmony.vts.test.vm.jni.object_fields;

import org.apache.harmony.vts.test.vm.jni.share.JNITest;

/**
 * @author Gregory Shimansky
 *
 * Test for GetObjectField function.
 */
public class GetObjectFieldTest extends JNITest {
    private native Object nativeExecute(Object obj, String field);

    /**
     * Native code returns value of the object field specified
     * in the argument. Test checks that this value is equal
     * to the field value.
     * @see org.apache.harmony.vts.test.vm.jni.share.JNITest#execute()
     */
    public boolean execute() throws Exception {
        TestClass tc = new TestClass();
        Object obj1 = new Object(), obj2 = new Object(), obj3 = new Object(); 

        tc.setLpub(obj1);
        tc.setLprot(obj2);
        tc.setLpriv(obj3);
        Object res1 = nativeExecute(tc, "lpub");
        Object res2 = nativeExecute(tc, "lprot");
        Object res3 = nativeExecute(tc, "lpriv");

        return res1 == obj1 && res2 == obj2 && res3 == obj3;
    }
    public static void main(String[] args){
        System.exit(new GetObjectFieldTest().test());
    }
}