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
* @author Vera Y. Petrashkova
* @version $Revision$
*/

package java.security.cert;

import java.security.cert.CRLException;

import junit.framework.TestCase;


/**
 * Tests for <code>CRLException</code> class constructors and methods.
 * 
 */
public class CRLExceptionTest extends TestCase {

    public static void main(String[] args) {
    }

    /**
     * Constructor for CRLExceptionTests.
     * 
     * @param arg0
     */
    public CRLExceptionTest(String arg0) {
        super(arg0);
    }

    private static String[] msgs = {
            "",
            "Check new message",
            "Check new message Check new message Check new message Check new message Check new message" };

    private static Throwable tCause = new Throwable("Throwable for exception");

    static String createErr(Exception tE, Exception eE) {
        return "CRLException: ".concat(tE.toString()).concat(
                " is not equal to caught exception: ").concat(eE.toString());
    }

    /**
     * Test for <code>CRLException()</code> constructor Assertion: constructs
     * CRLException with no detail message
     */
    public void testCRLException01() {
        CRLException tE = new CRLException();
        assertNull("getMessage() must return null.", tE.getMessage());
        assertNull("getCause() must return null", tE.getCause());
        try {
            throw tE;
        } catch (Exception e) {
            assertTrue(createErr(tE, e), tE.equals(e));
        }
    }

    /**
     * Test for <code>CRLException(String)</code> constructor Assertion:
     * constructs CRLException with detail message msg. Parameter
     * <code>msg</code> is not null.
     */
    public void testCRLException02() {
        CRLException tE;
        for (int i = 0; i < msgs.length; i++) {
            tE = new CRLException(msgs[i]);
            assertEquals("getMessage() must return: ".concat(msgs[i]), tE
                    .getMessage(), msgs[i]);
            assertNull("getCause() must return null", tE.getCause());
            try {
                throw tE;
            } catch (Exception e) {
                assertTrue(createErr(tE, e), tE.equals(e));
            }
        }
    }

    /**
     * Test for <code>CRLException(String)</code> constructor Assertion:
     * constructs CRLException when <code>msg</code> is null
     */
    public void testCRLException03() {
        String msg = null;
        CRLException tE = new CRLException(msg);
        assertNull("getMessage() must return null.", tE.getMessage());
        assertNull("getCause() must return null", tE.getCause());
        try {
            throw tE;
        } catch (Exception e) {
            assertTrue(createErr(tE, e), tE.equals(e));
        }
    }

    /**
     * Test for <code>CRLException(Throwable)</code> constructor Assertion:
     * constructs CRLException when <code>cause</code> is null
     */
    public void testCRLException04() {
        Throwable cause = null;
        CRLException tE = new CRLException(cause);
        assertNull("getMessage() must return null.", tE.getMessage());
        assertNull("getCause() must return null", tE.getCause());
        try {
            throw tE;
        } catch (Exception e) {
            assertTrue(createErr(tE, e), tE.equals(e));
        }
    }

    /**
     * Test for <code>CRLException(Throwable)</code> constructor Assertion:
     * constructs CRLException when <code>cause</code> is not null
     */
    public void testCRLException05() {
        CRLException tE = new CRLException(tCause);
        if (tE.getMessage() != null) {
            String toS = tCause.toString();
            String getM = tE.getMessage();
            assertTrue("getMessage() should contain ".concat(toS), (getM
                    .indexOf(toS) != -1));
        }
        assertNotNull("getCause() must not return null", tE.getCause());
        assertEquals("getCause() must return ".concat(tCause.toString()), tE
                .getCause(), tCause);
        try {
            throw tE;
        } catch (Exception e) {
            assertTrue(createErr(tE, e), tE.equals(e));
        }
    }

    /**
     * Test for <code>CRLException(String, Throwable)</code> constructor
     * Assertion: constructs CRLException when <code>cause</code> is null
     * <code>msg</code> is null
     */
    public void testCRLException06() {
        CRLException tE = new CRLException(null, null);
        assertNull("getMessage() must return null", tE.getMessage());
        assertNull("getCause() must return null", tE.getCause());
        try {
            throw tE;
        } catch (Exception e) {
            assertTrue(createErr(tE, e), tE.equals(e));
        }
    }

    /**
     * Test for <code>CRLException(String, Throwable)</code> constructor
     * Assertion: constructs CRLException when <code>cause</code> is null
     * <code>msg</code> is not null
     */
    public void testCRLException07() {
        CRLException tE;
        for (int i = 0; i < msgs.length; i++) {
            tE = new CRLException(msgs[i], null);
            assertEquals("getMessage() must return: ".concat(msgs[i]), tE
                    .getMessage(), msgs[i]);
            assertNull("getCause() must return null", tE.getCause());
            try {
                throw tE;
            } catch (Exception e) {
                assertTrue(createErr(tE, e), tE.equals(e));
            }
        }
    }

    /**
     * Test for <code>CRLException(String, Throwable)</code> constructor
     * Assertion: constructs CRLException when <code>cause</code> is not null
     * <code>msg</code> is null
     */
    public void testCRLException08() {
        CRLException tE = new CRLException(null, tCause);
        if (tE.getMessage() != null) {
            String toS = tCause.toString();
            String getM = tE.getMessage();
            assertTrue("getMessage() must should ".concat(toS), (getM
                    .indexOf(toS) != -1));
        }
        assertNotNull("getCause() must not return null", tE.getCause());
        assertEquals("getCause() must return ".concat(tCause.toString()), tE
                .getCause(), tCause);
        try {
            throw tE;
        } catch (Exception e) {
            assertTrue(createErr(tE, e), tE.equals(e));
        }
    }

    /**
     * Test for <code>CRLException(String, Throwable)</code> constructor
     * Assertion: constructs CRLException when <code>cause</code> is not null
     * <code>msg</code> is not null
     */
    public void testCRLException09() {
        CRLException tE;
        for (int i = 0; i < msgs.length; i++) {
            tE = new CRLException(msgs[i], tCause);
            String getM = tE.getMessage();
            String toS = tCause.toString();
            if (msgs[i].length() > 0) {
                assertTrue("getMessage() must contain ".concat(msgs[i]), getM
                        .indexOf(msgs[i]) != -1);
                if (!getM.equals(msgs[i])) {
                    assertTrue("getMessage() should contain ".concat(toS), getM
                            .indexOf(toS) != -1);
                }
            }
            assertNotNull("getCause() must not return null", tE.getCause());
            assertEquals("getCause() must return ".concat(tCause.toString()),
                    tE.getCause(), tCause);

            try {
                throw tE;
            } catch (Exception e) {
                assertTrue(createErr(tE, e), tE.equals(e));
            }
        }
    }
}
