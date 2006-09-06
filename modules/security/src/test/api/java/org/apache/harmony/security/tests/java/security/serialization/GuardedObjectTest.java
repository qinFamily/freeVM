/*
 *  Copyright 2005 The Apache Software Foundation or its licensors, as applicable.
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
* @author Alexey V. Varlamov
* @version $Revision$
*/

package org.apache.harmony.security.tests.java.security.serialization;

import java.io.Serializable;
import java.security.GuardedObject;

import org.apache.harmony.security.tests.support.MyGuard;
import org.apache.harmony.testframework.serialization.SerializationTest;

/**
 * Serialization tests for <code>GuardedObject</code>
 */

public class GuardedObjectTest extends SerializationTest implements
        SerializationTest.SerializableAssert {

    /**
     * @see com.intel.drl.test.SerializationTest#getData()
     */
    protected Object[] getData() {
        return new Object[] { new GuardedObject(null, null),
                new GuardedObject("dsgdfg", null),
                new GuardedObject(new Integer(76547), new MyGuard(true)), };
    }

    public void assertDeserialized(Serializable golden, Serializable test) {
        assertSame(golden.getClass(), test.getClass());
        assertEquals(((GuardedObject) golden).getObject(),
                ((GuardedObject) test).getObject());
    }

    public void testDisableGuard() throws Throwable {
        try {
            copySerializable(new GuardedObject(null, new MyGuard(false)));
            fail("Should not serialize if guard denies access");
        }
        catch (SecurityException ok) {}
    }
}
