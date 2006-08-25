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

import java.sql.SQLWarning;

import junit.framework.TestCase;

public class SQLWarningTest extends TestCase {

	/*
	 * ConstructorTest
	 */
	public void testSQLWarning() {

		String[] theFinalStates1 = { null };
		String[] theFinalStates2 = { null };
		int[] theFinalStates3 = { 0 };
		SQLWarning[] theFinalStates4 = { null };

		Exception[] theExceptions = { null };

		SQLWarning aSQLWarning;
		int loopCount = 1;
		for (int i = 0; i < loopCount; i++) {
			try {
				aSQLWarning = new SQLWarning();
				if (theExceptions[i] != null)
					assertTrue(false);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getMessage(), theFinalStates1[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getSQLState(), theFinalStates2[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getErrorCode(), theFinalStates3[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getNextWarning(), theFinalStates4[i]);

			} catch (Exception e) {
				if (theExceptions[i] == null)
					fail(i + "Unexpected exception");
				assertEquals(i + "Exception mismatch", e.getClass(),
						theExceptions[i].getClass());
				assertEquals(i + "Exception mismatch", e.getMessage(),
						theExceptions[i].getMessage());
			} // end try
		} // end for

	} // end method testSQLWarning

	/*
	 * ConstructorTest
	 */
	public void testSQLWarningString() {

		String[] init1 = { "a", "1", "valid1", "----", "&valid*", null, "", " " };

		String[] theFinalStates1 = init1;
		String[] theFinalStates2 = { null, null, null, null, null, null, null,
				null };
		int[] theFinalStates3 = { 0, 0, 0, 0, 0, 0, 0, 0 };
		SQLWarning[] theFinalStates4 = { null, null, null, null, null, null,
				null, null };

		Exception[] theExceptions = { null, null, null, null, null, null, null,
				null };

		SQLWarning aSQLWarning;
		int loopCount = init1.length;
		for (int i = 0; i < loopCount; i++) {
			try {
				aSQLWarning = new SQLWarning(init1[i]);
				if (theExceptions[i] != null)
					assertTrue(false);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getMessage(), theFinalStates1[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getSQLState(), theFinalStates2[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getErrorCode(), theFinalStates3[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getNextWarning(), theFinalStates4[i]);

			} catch (Exception e) {
				if (theExceptions[i] == null)
					fail(i + "Unexpected exception");
				assertEquals(i + "Exception mismatch", e.getClass(),
						theExceptions[i].getClass());
				assertEquals(i + "Exception mismatch", e.getMessage(),
						theExceptions[i].getMessage());
			} // end try
		} // end for

	} // end method testSQLWarningString

	/*
	 * ConstructorTest
	 */
	public void testSQLWarningStringString() {

		String[] init1 = { "a", "1", "valid1", "----", "&valid*", null, "",
				" ", "a", "a", "a" };
		String[] init2 = { "a", "1", "valid1", "----", "&valid*", "a", "a",
				"a", null, "", " " };

		String[] theFinalStates1 = init1;
		String[] theFinalStates2 = init2;
		int[] theFinalStates3 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		SQLWarning[] theFinalStates4 = { null, null, null, null, null, null,
				null, null, null, null, null };

		Exception[] theExceptions = { null, null, null, null, null, null, null,
				null, null, null, null };

		SQLWarning aSQLWarning;
		int loopCount = init1.length;
		for (int i = 0; i < loopCount; i++) {
			try {
				aSQLWarning = new SQLWarning(init1[i], init2[i]);
				if (theExceptions[i] != null)
					assertTrue(false);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getMessage(), theFinalStates1[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getSQLState(), theFinalStates2[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getErrorCode(), theFinalStates3[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getNextWarning(), theFinalStates4[i]);

			} catch (Exception e) {
				if (theExceptions[i] == null)
					fail(i + "Unexpected exception");
				assertEquals(i + "Exception mismatch", e.getClass(),
						theExceptions[i].getClass());
				assertEquals(i + "Exception mismatch", e.getMessage(),
						theExceptions[i].getMessage());
			} // end try
		} // end for

	} // end method testSQLWarningStringString

	/*
	 * ConstructorTest
	 */
	public void testSQLWarningStringStringint() {

		String[] init1 = { "a", "1", "valid1", "----", "&valid*", "----",
				"----", null, "", " ", "a", "a", "a" };
		String[] init2 = { "a", "1", "valid1", "----", "&valid*", "valid1",
				"----", "a", "a", "a", null, "", " " };
		int[] init3 = { -2147483648, 2147483647, 0, 1412862821, -733923487,
				488067774, -1529953616, -2147483648, -2147483648, -2147483648,
				-2147483648, -2147483648, -2147483648 };

		String[] theFinalStates1 = init1;
		String[] theFinalStates2 = init2;
		int[] theFinalStates3 = init3;
		SQLWarning[] theFinalStates4 = { null, null, null, null, null, null,
				null, null, null, null, null, null, null };

		Exception[] theExceptions = { null, null, null, null, null, null, null,
				null, null, null, null, null, null };

		SQLWarning aSQLWarning;
		int loopCount = init1.length;
		for (int i = 0; i < loopCount; i++) {
			try {
				aSQLWarning = new SQLWarning(init1[i], init2[i], init3[i]);
				if (theExceptions[i] != null)
					assertTrue(false);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getMessage(), theFinalStates1[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getSQLState(), theFinalStates2[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getErrorCode(), theFinalStates3[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getNextWarning(), theFinalStates4[i]);

			} catch (Exception e) {
				if (theExceptions[i] == null)
					fail(i + "Unexpected exception");
				assertEquals(i + "Exception mismatch", e.getClass(),
						theExceptions[i].getClass());
				assertEquals(i + "Exception mismatch", e.getMessage(),
						theExceptions[i].getMessage());
			} // end try
		} // end for

	} // end method testSQLWarningStringStringint

	/*
	 * Method test for getNextWarning
	 */
	public void testGetNextWarning() {

		SQLWarning aSQLWarning;
		String[] init1 = { "a", "1", "valid1", "----", "&valid*" };

		SQLWarning theReturn;
		SQLWarning[] theReturns = { null };
		String[] theFinalStates1 = init1;
		String[] theFinalStates2 = { null };
		int[] theFinalStates3 = { 0 };
		SQLWarning[] theFinalStates4 = { null };

		Exception[] theExceptions = { null };

		int loopCount = 1;
		for (int i = 0; i < loopCount; i++) {
			try {
				aSQLWarning = new SQLWarning(init1[i]);
				theReturn = aSQLWarning.getNextWarning();
				if (theExceptions[i] != null)
					fail(i + "Exception missed");
				assertEquals(i + "Return value mismatch", theReturn,
						theReturns[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getMessage(), theFinalStates1[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getSQLState(), theFinalStates2[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getErrorCode(), theFinalStates3[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getNextWarning(), theFinalStates4[i]);

			} catch (Exception e) {
				if (theExceptions[i] == null)
					fail(i + "Unexpected exception");
				assertEquals(i + "Exception mismatch", e.getClass(),
						theExceptions[i].getClass());
				assertEquals(i + "Exception mismatch", e.getMessage(),
						theExceptions[i].getMessage());
			} // end try
		} // end for

	} // end method testGetNextWarning

	/*
	 * Method test for setNextWarning
	 */
	public void testSetNextWarningSQLWarning() {

		SQLWarning[] parm1 = { new SQLWarning(), null };

		SQLWarning aSQLWarning;
		String[] init1 = { "a", "1" };

		String[] theFinalStates1 = init1;
		String[] theFinalStates2 = { null, null };
		int[] theFinalStates3 = { 0, 0 };
		SQLWarning[] theFinalStates4 = parm1;

		Exception[] theExceptions = { null, null };

		int loopCount = parm1.length;
		for (int i = 0; i < loopCount; i++) {
			try {
				aSQLWarning = new SQLWarning(init1[i]);
				aSQLWarning.setNextWarning(parm1[i]);
				if (theExceptions[i] != null)
					fail(i + "Exception missed");
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getMessage(), theFinalStates1[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getSQLState(), theFinalStates2[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getErrorCode(), theFinalStates3[i]);
				assertEquals(i + "  Final state mismatch", aSQLWarning
						.getNextWarning(), theFinalStates4[i]);

			} catch (Exception e) {
				if (theExceptions[i] == null)
					fail(i + "Unexpected exception");
				assertEquals(i + "Exception mismatch", e.getClass(),
						theExceptions[i].getClass());
				assertEquals(i + "Exception mismatch", e.getMessage(),
						theExceptions[i].getMessage());
			} // end try
		} // end for

	} // end method testSetNextWarningSQLWarning

} // end class SQLWarningTest

