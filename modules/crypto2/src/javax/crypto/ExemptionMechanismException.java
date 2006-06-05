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

package javax.crypto;

import java.security.GeneralSecurityException;

/**
 * 
 * 
 * @author Diego Ra�l Mercado
 * @version 1.2
 * @ar.org.fitc.spec_ref
 */
public class ExemptionMechanismException extends GeneralSecurityException {
    private static final long serialVersionUID = 1572699429277957109L;

    /** @ar.org.fitc.spec_ref */
    public ExemptionMechanismException() {
        super();
    }

    /** @ar.org.fitc.spec_ref */
    public ExemptionMechanismException(String msg) {
        super(msg);
    }
}
