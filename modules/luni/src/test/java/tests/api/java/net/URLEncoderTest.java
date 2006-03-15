/* Copyright 1998, 2005 The Apache Software Foundation or its licensors, as applicable
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

package tests.api.java.net;

import java.net.URLDecoder;
import java.net.URLEncoder;

import tests.support.Support_Configuration;

public class URLEncoderTest extends junit.framework.TestCase {

	/**
	 * @tests java.net.URLEncoder#encode(java.lang.String)
	 */
	public void test_encodeLjava_lang_String() {
		// Test for method java.lang.String
		// java.net.URLEncoder.encode(java.lang.String)
		final String URL = "http://" + Support_Configuration.HomeAddress;
		final String URL2 = "telnet://justWantToHaveFun.com:400";
		final String URL3 = "file://myServer.org/a file with spaces.jpg";
		try {
			assertTrue("1. Incorrect encoding/decoding", URLDecoder.decode(
					URLEncoder.encode(URL)).equals(URL));
			assertTrue("2. Incorrect encoding/decoding", URLDecoder.decode(
					URLEncoder.encode(URL2)).equals(URL2));
			assertTrue("3. Incorrect encoding/decoding", URLDecoder.decode(
					URLEncoder.encode(URL3)).equals(URL3));
		} catch (Exception e) {
			fail("Exception during test : " + e.getMessage());
		}
	}

	/**
	 * Sets up the fixture, for example, open a network connection. This method
	 * is called before a test is executed.
	 */
	protected void setUp() {
	}

	/**
	 * Tears down the fixture, for example, close a network connection. This
	 * method is called after a test is executed.
	 */
	protected void tearDown() {
	}
}
