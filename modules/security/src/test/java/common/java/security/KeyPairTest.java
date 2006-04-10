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
* @author Vladimir N. Molotkov
* @version $Revision$
*/

package java.security;

import java.security.spec.InvalidKeySpecException;

import junit.framework.TestCase;


/**
 * Tests for fields and methods of class <code>KeyPair</code>
 * 
 */
public class KeyPairTest extends TestCase {

    private static class TestKeyPair {
        static PublicKey getPublic() {
            return new PublicKey() {
                public String getAlgorithm() {
                    return "never mind";
                }
                public String getFormat() {
                    return "never mind";
                }
                public byte[] getEncoded() {
                    return null;
                }                
            };
        }
        static PrivateKey getPrivate() {
            return new PrivateKey() {
                public String getAlgorithm() {
                    return "never mind";
                }
                public String getFormat() {
                    return "never mind";
                }
                public byte[] getEncoded() {
                    return null;
                }                
            };                
        }
    }

    /**
     * Constructor for KeyPairTest.
     * @param name
     */
    public KeyPairTest(String name) {
        super(name);
    }

    /**
     * Test #1 for <code>KeyPair(PublicKey, PrivateKey)</code> constructor<br>
     * Assertion: creates new <code>KeyPair</code> instance using valid
     * parameters (both <code>null</code>)
     */
    public final void testKeyPair01() {
        Object kp = new KeyPair(null, null);
        assertTrue(kp instanceof KeyPair);
    }

    /**
     * Test #2 for <code>KeyPair(PublicKey, PrivateKey)</code> constructor<br>
     * Assertion: creates new <code>KeyPair</code> instance using valid
     * parameters (both valid keys)
     * @throws InvalidKeySpecException
     */
    public final void testKeyPair02() throws InvalidKeySpecException {
        Object kp = new KeyPair(TestKeyPair.getPublic(), TestKeyPair.getPrivate());
        assertTrue(kp instanceof KeyPair);
    }

    /**
     * Test #1 for <code>getPrivate()</code> method<br>
     * Assertion: returns private key (<code>null</code> in this case)
     */
    public final void testGetPrivate01() {
        KeyPair kp = new KeyPair(null, null);
        assertTrue(kp.getPrivate() == null);
    }

    /**
     * Test #2 for <code>getPrivate()</code> method<br>
     * Assertion: returns private key (valid private key in this case)
     * @throws InvalidKeySpecException
     */
    public final void testGetPrivate02() throws InvalidKeySpecException {
        PrivateKey pk = TestKeyPair.getPrivate();
        KeyPair kp = new KeyPair(null, pk);
        assertSame(pk, kp.getPrivate());
    }

    /**
     * Test #1 for <code>getPublic()</code> method<br>
     * Assertion: returns public key (<code>null</code> in this case)
     */
    public final void testGetPublic01() {
        KeyPair kp = new KeyPair(null, null);
        assertTrue(kp.getPublic() == null);
    }

    /**
     * Test #2 for <code>getPublic()</code> method<br>
     * Assertion: returns public key (valid public key in this case)
     * @throws InvalidKeySpecException
     */
    public final void testGetPublic02() throws InvalidKeySpecException {
        PublicKey pk = TestKeyPair.getPublic();
        KeyPair kp = new KeyPair(pk, null);
        assertSame(pk, kp.getPublic());
    }

}
