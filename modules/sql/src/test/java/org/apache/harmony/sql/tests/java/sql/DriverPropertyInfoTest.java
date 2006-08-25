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

package org.apache.harmony.sql.tests.java.sql;

import java.sql.DriverPropertyInfo;

import junit.framework.TestCase;

/**
 * JUnit Testcase for the java.sql.DriverPropertyInfo class
 * 
 */

public class DriverPropertyInfoTest extends TestCase {

	/*
	 * Public statics test
	 */
	public void testPublicStatics() {

	} // end method testPublicStatics

	/*
	 * Constructor test
	 */
	public void testDriverPropertyInfoStringString() {

		DriverPropertyInfo aDriverPropertyInfo = new DriverPropertyInfo(
				validName, validValue);

		assertNotNull(aDriverPropertyInfo);

		aDriverPropertyInfo = new DriverPropertyInfo(null, null);

	} // end method testDriverPropertyInfoStringString

	/*
	 * Public fields test
	 */
	static String validName = "testname";

	static String validValue = "testvalue";

	static String[] updateChoices = { "Choice1", "Choice2", "Choice3" };

	static String updateValue = "updateValue";

	static boolean updateRequired = true;

	static String updateDescription = "update description";

	static String updateName = "updateName";

	public void testPublicFields() {

		// Constructor here...
		DriverPropertyInfo aDriverPropertyInfo = new DriverPropertyInfo(
				validName, validValue);

		assertTrue(aDriverPropertyInfo.choices == testChoices);
		assertTrue(aDriverPropertyInfo.value == testValue);
		assertTrue(aDriverPropertyInfo.required == testRequired);
		assertTrue(aDriverPropertyInfo.description == testDescription);
		assertTrue(aDriverPropertyInfo.name == testName);

		aDriverPropertyInfo.choices = updateChoices;
		aDriverPropertyInfo.value = updateValue;
		aDriverPropertyInfo.required = updateRequired;
		aDriverPropertyInfo.description = updateDescription;
		aDriverPropertyInfo.name = updateName;

		assertTrue(aDriverPropertyInfo.choices == updateChoices);
		assertTrue(aDriverPropertyInfo.value == updateValue);
		assertTrue(aDriverPropertyInfo.required == updateRequired);
		assertTrue(aDriverPropertyInfo.description == updateDescription);
		assertTrue(aDriverPropertyInfo.name == updateName);

	} // end method testPublicFields

	// Default values...
	static String[] testChoices = null;

	static java.lang.String testValue = validValue;

	static boolean testRequired = false;

	static java.lang.String testDescription = null;

	static java.lang.String testName = validName;

} // end class DriverPropertyInfoTest
