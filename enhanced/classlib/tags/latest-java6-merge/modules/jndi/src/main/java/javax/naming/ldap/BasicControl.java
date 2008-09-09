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

package javax.naming.ldap;

import javax.naming.ldap.Control;

/**
 * TODO: JavaDoc
 */
public class BasicControl implements Control {

    private static final long serialVersionUID = -4233907508771791687L;

    protected String id;

    protected boolean criticality = false;

    protected byte[] value;

    public BasicControl(String id) {
        this.id = id;
    }

    public BasicControl(String id, boolean criticality, byte[] value) {
        this.id = id;
        this.criticality = criticality;
        this.value = value;
    }

    public byte[] getEncodedValue() {
        return value;
    }

    public boolean isCritical() {
        return criticality;
    }

    public String getID() {
        return id;
    }

}
