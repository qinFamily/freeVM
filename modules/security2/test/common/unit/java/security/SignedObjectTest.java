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
* @author Boris V. Kuznetsov
* @version $Revision$
*/

package java.security;

import java.io.IOException;
import java.security.spec.InvalidKeySpecException;
import java.util.Properties;

import org.apache.harmony.security.TestKeyPair;

import com.openintel.drl.security.test.PerformanceTest;

/**
 * Tests for <code>SignedObject</code> constructor and methods
 * 
 */
public class SignedObjectTest extends PerformanceTest {

	public void testSignedObject() {
		Signature sig = null;
		TestKeyPair tkp = null;
	    Properties prop;
	    
	    try {
        	sig = Signature.getInstance("SHA1withDSA");		
    	} catch (NoSuchAlgorithmException e) {
    		fail(e.toString());
    	}
    	
    	try {
			tkp = new TestKeyPair("DSA");
		} catch (NoSuchAlgorithmException e) {
    		e.printStackTrace();
    		return;
		}
    	prop = new Properties();
    	prop.put("aaa", "bbb");
    	SignedObject so = null;
    	try {
    		so = new SignedObject(prop, tkp.getPrivate(), sig);
    	} catch (IOException e) {
           	fail(e.toString());  
    	} catch (SignatureException e) {   
           	fail(e.toString());  
    	} catch (InvalidKeyException e) {
           	fail(e.toString());  
    	} catch (InvalidKeySpecException e) {
          	fail(e.toString());
		}

    	assertEquals("SHA1withDSA", so.getAlgorithm());
 
        try {
            assertEquals((Properties)so.getObject(), prop);      	
        } catch (ClassNotFoundException e) {
           	fail(e.toString());  
        } catch (IOException e) {
           	fail(e.toString());  
        }
        try {
        	if (!so.verify(tkp.getPublic(), sig)) {
            	fail("verify() failed");
            }	
        } catch (SignatureException e) {
        	fail(e.toString());      	
        } catch (InvalidKeyException e) {
           	fail(e.toString());         	
        } catch (InvalidKeySpecException e) {
           	fail(e.toString()); 
		}
        
        if (so.getSignature() == null) {
        	fail("signature is null");
        }	    	 
	}
	
}
