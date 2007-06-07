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

package javax.print.attribute.standard;

import javax.print.attribute.Attribute;
import javax.print.attribute.SetOfIntegerSyntax;
import javax.print.attribute.SupportedValuesAttribute;

public final class CopiesSupported extends SetOfIntegerSyntax implements
        SupportedValuesAttribute {
    private static final long serialVersionUID = 6927711687034846001L;

    public CopiesSupported(int value) {
        super(value);
        if (value < 1) {
            throw new IllegalArgumentException("Value" + value + "is less than 1");
        }
    }

    public CopiesSupported(int lowerBound, int upperBound) {
        super(lowerBound, upperBound);
        if (lowerBound > upperBound) {
            throw new IllegalArgumentException("Null range");
        } else if (lowerBound < 1) {
            throw new IllegalArgumentException("Lower bound" + lowerBound + "is less than 1");
        }
    }

    @Override
    public boolean equals(Object object) {
        if (!(object instanceof CopiesSupported)) {
            return false;
        }
        return super.equals(object);
    }

    public Class<? extends Attribute> getCategory() {
        return CopiesSupported.class;
    }

    public String getName() {
        return "copies-supported";
    }
}
