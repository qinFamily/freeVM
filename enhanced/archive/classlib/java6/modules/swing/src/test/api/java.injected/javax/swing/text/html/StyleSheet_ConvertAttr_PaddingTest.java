/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
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
 * @author Alexey A. Ivanov
 * @version $Revision$
 */
package javax.swing.text.html;

public class StyleSheet_ConvertAttr_PaddingTest
    extends StyleSheet_ConvertAttr_SpaceTestCase {

    protected void setUp() throws Exception {
        super.setUp();
        shorthandKey = CSS.Attribute.PADDING;
        topKey = CSS.Attribute.PADDING_TOP;
        rightKey = CSS.Attribute.PADDING_RIGHT;
        bottomKey = CSS.Attribute.PADDING_BOTTOM;
        leftKey = CSS.Attribute.PADDING_LEFT;

        defaultValue = "0";
    }
}