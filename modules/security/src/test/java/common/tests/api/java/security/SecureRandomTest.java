/* Copyright 2005 The Apache Software Foundation or its licensors, as applicable
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package tests.api.java.security;

import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.Provider;
import java.security.SecureRandom;
import java.security.Security;

public class SecureRandomTest extends junit.framework.TestCase {

	private static final byte[] SEED_BYTES = { (byte) 33, (byte) 15, (byte) -3,
			(byte) 22, (byte) 77, (byte) -16, (byte) -33, (byte) 56 };

	private static final int SEED_SIZE = 539;

	private static final long SEED_VALUE = 5335486759L;

	/**
	 * @tests java.security.SecureRandom#SecureRandom()
	 */
	public void test_Constructor() {
		// Test for method java.security.SecureRandom()
		try {
			new SecureRandom();
		} catch (Exception e) {
			fail("Constructor threw exception : " + e);
		}
	}

	/**
	 * @tests java.security.SecureRandom#SecureRandom(byte[])
	 */
	public void test_Constructor$B() {
		// Test for method java.security.SecureRandom(byte [])
		try {
			new SecureRandom(SEED_BYTES);
		} catch (Exception e) {
			fail("Constructor threw exception : " + e);
		}
	}

	/**
	 * @tests java.security.SecureRandom#generateSeed(int)
	 */
	public void test_generateSeedI() {
		// Test for method byte [] java.security.SecureRandom.generateSeed(int)
		byte[] seed = new SecureRandom().generateSeed(SEED_SIZE);
		assertEquals("seed has incorrect size", SEED_SIZE, seed.length);
	}

	/**
	 * @tests java.security.SecureRandom#getInstance(java.lang.String)
	 */
	public void test_getInstanceLjava_lang_String() {
		// Test for method java.security.SecureRandom
		// java.security.SecureRandom.getInstance(java.lang.String)
		try {
			SecureRandom.getInstance("SHA1PRNG");
		} catch (NoSuchAlgorithmException e) {
			fail("getInstance did not find a SHA1PRNG algorithm");
		}
	}

	/**
	 * @tests java.security.SecureRandom#getInstance(java.lang.String,
	 *        java.lang.String)
	 */
	public void test_getInstanceLjava_lang_StringLjava_lang_String() {
		// Test for method java.security.SecureRandom
		// java.security.SecureRandom.getInstance(java.lang.String,
		// java.lang.String)
		try {
			Provider[] providers = Security
					.getProviders("SecureRandom.SHA1PRNG");
			if (providers != null) {
				for (int i = 0; i < providers.length; i++) {
					SecureRandom
							.getInstance("SHA1PRNG", providers[i].getName());
				}// end for
			} else {
				fail("No providers support SHA1PRNG");
			}
		} catch (NoSuchAlgorithmException e) {
			fail("getInstance did not find a SHA1PRNG algorithm");
		} catch (NoSuchProviderException e) {
			fail("getInstance did not find the provider for SHA1PRNG");
		}
	}

	/**
	 * @tests java.security.SecureRandom#getProvider()
	 */
	public void test_getProvider() {
		// Test for method java.security.Provider
		// java.security.SecureRandom.getProvider()
		Provider p = new SecureRandom().getProvider();
		assertNotNull("SecureRandom provider is null", p);
	}

	/**
	 * @tests java.security.SecureRandom#getSeed(int)
	 */
	public void test_getSeedI() {
		// Test for method byte [] java.security.SecureRandom.getSeed(int)
		byte[] seed = SecureRandom.getSeed(SEED_SIZE);
		assertEquals("seed has incorrect size", SEED_SIZE, seed.length);
	}

	/**
	 * @tests java.security.SecureRandom#nextBytes(byte[])
	 */
	public void test_nextBytes$B() {
		// Test for method void java.security.SecureRandom.nextBytes(byte [])
		byte[] bytes = new byte[313];
		try {
		new SecureRandom().nextBytes(bytes);
		} catch (Exception e) {
			fail("next bytes not ok : " + e);
		}
	}

	/**
	 * @tests java.security.SecureRandom#setSeed(byte[])
	 */
	public void test_setSeed$B() {
		// Test for method void java.security.SecureRandom.setSeed(byte [])
		try {
			new SecureRandom().setSeed(SEED_BYTES);
		} catch (Exception e) {
			fail("seed generation with bytes failed : " + e);
		}
	}

	/**
	 * @tests java.security.SecureRandom#setSeed(long)
	 */
	public void test_setSeedJ() {
		// Test for method void java.security.SecureRandom.setSeed(long)
		try {
			new SecureRandom().setSeed(SEED_VALUE);
		} catch (Exception e) {
			fail("seed generation with long failed : " + e);
		}
	}
}