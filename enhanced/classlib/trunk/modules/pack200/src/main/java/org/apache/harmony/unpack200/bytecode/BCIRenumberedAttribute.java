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
package org.apache.harmony.unpack200.bytecode;

import java.io.DataOutputStream;
import java.io.IOException;
import java.util.List;

import org.apache.harmony.pack200.Pack200Exception;

/**
 * Abstract superclass for attributes that have some part encoded with a BCI
 * renumbering
 */
public abstract class BCIRenumberedAttribute extends Attribute {

    protected boolean renumbered = false;

    /*
     * (non-Javadoc)
     *
     * @see org.apache.harmony.unpack200.bytecode.Attribute#hasBCIRenumbering()
     */
    public boolean hasBCIRenumbering() {
        return true;
    }

    public BCIRenumberedAttribute(CPUTF8 attributeName) {
        super(attributeName);
    }

    protected abstract int getLength();

    protected abstract void writeBody(DataOutputStream dos) throws IOException;

    public abstract String toString();

    protected abstract int[] getStartPCs();

    /**
     * In Pack200, line number tables are BCI renumbered. This method takes the
     * byteCodeOffsets (which is a List of Integers specifying the offset in the
     * byte code array of each instruction) and updates the start_pcs so that it
     * points to the instruction index itself, not the BCI renumbering of the
     * instruction.
     *
     * @param byteCodeOffsets
     *            List of Integer offsets of the bytecode array
     */
    public void renumber(List byteCodeOffsets) throws Pack200Exception {
        if (renumbered) {
            throw new Error(
                    "Trying to renumber a line number table that has already been renumbered");
        }
        renumbered = true;
        int[] startPCs = getStartPCs();
        for (int index = 0; index < startPCs.length; index++) {
            startPCs[index] = ((Integer) byteCodeOffsets.get(startPCs[index]))
                    .intValue();
        }
    }

}
