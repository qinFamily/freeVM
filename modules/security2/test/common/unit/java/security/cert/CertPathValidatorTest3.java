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


import java.security.InvalidAlgorithmParameterException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.Provider;

import org.apache.harmony.security.SpiEngUtils;

import com.openintel.drl.security.cert.MyCertPath;
import com.openintel.drl.security.cert.TestUtils;
import com.openintel.drl.security.test.PerformanceTest;
/**
 * Tests for <code>CertPathValidator</code> class  methods.
 * 
 */

public class CertPathValidatorTest3 extends PerformanceTest {

    /**
     * Constructor for CertPathValidatorTests.
     * @param name
     */
    public CertPathValidatorTest3(String name) {
        super(name);
    }
    private static final String defaultType = CertPathBuilderTest1.defaultType;    
    private static final String [] validValues = CertPathBuilderTest1.validValues;
     
    private static String [] invalidValues = SpiEngUtils.invalidValues;
    
    private static boolean PKIXSupport = false;

    private static Provider defaultProvider;
    private static String defaultProviderName;
    
    private static String NotSupportMsg = "";

    static {
        defaultProvider = SpiEngUtils.isSupport(defaultType,
                CertPathValidatorTest1.srvCertPathValidator);
        PKIXSupport = (defaultProvider != null);
        defaultProviderName = (PKIXSupport ? defaultProvider.getName() : null);
        NotSupportMsg = defaultType.concat(" is not supported");
    }
    
    private static CertPathValidator[] createCPVs() {
        if (!PKIXSupport) {
            fail(NotSupportMsg);
            return null;
        }
        try {
            CertPathValidator[] certPVs = new CertPathValidator[3];
            certPVs[0] = CertPathValidator.getInstance(defaultType);
            certPVs[1] = CertPathValidator.getInstance(defaultType,
                    defaultProviderName);
            certPVs[2] = CertPathValidator.getInstance(defaultType,
                    defaultProvider);
            return certPVs;
        } catch (Exception e) {
            return null;
        }
    }    
    /**
     * Test for <code>validate(CertPath certpath, CertPathParameters params)</code> method
	 * Assertion: throws InvalidAlgorithmParameterException 
	 * when params is instance of PKIXParameters and
	 * certpath is not X.509 type
	 * 
	 * FIXME: jrockit-j2re1.4.2_04 throws NullPointerException when certPath is null
     */
    public void testValidate01()
            throws NoSuchAlgorithmException, NoSuchProviderException, 
                    CertPathValidatorException, InvalidAlgorithmParameterException  {
        if (!PKIXSupport) {
            fail(NotSupportMsg);
            return;
        }
        MyCertPath mCP = new MyCertPath(new byte[0]);
        CertPathParameters params = (CertPathParameters)new PKIXParameters(TestUtils.getTrustAnchorSet()); 
        CertPathValidator [] certPV = createCPVs();
        assertNotNull("CertPathValidator objects were not created", certPV);
        for (int i = 0; i < certPV.length; i++) {            
            try {
                certPV[i].validate(mCP, null);
                fail("InvalidAlgorithmParameterException must be thrown");
            } catch(InvalidAlgorithmParameterException e) {
            }
            try {
                certPV[i].validate(null, params);
                fail("NullPointerException must be thrown");
            } catch(NullPointerException e) {
            }            
        }
    }

}
