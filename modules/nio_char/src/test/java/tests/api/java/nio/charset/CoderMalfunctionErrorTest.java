/* Copyright 2004 The Apache Software Foundation or its licensors, as applicable
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

package tests.api.java.nio.charset;

import java.nio.charset.CoderMalfunctionError;

import junit.framework.TestCase;
import tests.util.SerializationTester;

/**
 * Test java.nio.CoderMalfunctionError.
 */
public class CoderMalfunctionErrorTest extends TestCase {

	/*
	 * Test constructor with normal param.
	 */
	public void testConstructor_Normal() {
		Exception ex = new Exception();
		CoderMalfunctionError e = new CoderMalfunctionError(ex);
		assertSame(ex, e.getCause());
	}

	/*
	 * Test constructor with null param.
	 */
	public void testConstructor_Null() {
		CoderMalfunctionError e = new CoderMalfunctionError(null);
		assertNull(e.getCause());
	}

	/*
	 * Test serialization/deserialization.
	 */
	public void testSerialization() throws Exception {
		CoderMalfunctionError ex = new CoderMalfunctionError(null);

		CoderMalfunctionError deEx = (CoderMalfunctionError) SerializationTester
				.getDeserilizedObject(ex);
		assertEquals(null, deEx.getCause());
	}

	/*
	 * Test serialization/deserialization.
	 */
	public void testSerializationCompatibility() throws Exception {
		CoderMalfunctionError ex = new CoderMalfunctionError(null);

		CoderMalfunctionError deEx = (CoderMalfunctionError) SerializationTester
				.readObject(ex,
						"tests/api/java/nio/charset/CoderMalfunctionError.ser");
		assertEquals(null, deEx.getCause());
	}
}
