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

package tests.api.java.beans.beancontext;

import junit.framework.Test;
import junit.framework.TestSuite;

/**
 * Test for java.beans.beancontext.
 */
public class AllTests {

	public static void main(String[] args) {
		junit.textui.TestRunner.run(suite());
	}

	public static Test suite() {
		TestSuite suite = new TestSuite("Test for java.beans.beancontext");
		// $JUnit-BEGIN$
		suite.addTestSuite(BeanContextChildSupportTest.class);
		suite.addTestSuite(BeanContextEventTest.class);
		suite.addTestSuite(BeanContextMembershipEventTest.class);
		suite.addTestSuite(BeanContextServiceAvailableEventTest.class);
		suite.addTestSuite(BeanContextServiceRevokedEventTest.class);
		suite.addTestSuite(BeanContextServicesSupportTest.class);
		suite.addTestSuite(BeanContextSupportTest.class);
		suite.addTestSuite(InterfacesTest.class);
		// $JUnit-END$
		return suite;
	}
}
