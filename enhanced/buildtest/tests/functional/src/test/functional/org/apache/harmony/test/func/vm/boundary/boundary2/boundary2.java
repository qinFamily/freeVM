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
package org.apache.harmony.test.func.vm.boundary.boundary2;

import org.apache.harmony.share.Test;
import java.util.Random;

/**
 */

public class boundary2 extends Test {

    public static void main(String[] args) {
        System.exit(new boundary2().test(args));
    }

    public int test() {

        try {
            Random rr = new Random(69846L);            
            double[] fff = new double[6000000];
            for (int q = 0; q < 6000000; q++) {
                fff[q] = rr.nextDouble();
            }

            return pass();

        } catch (OutOfMemoryError OME) {
            return pass();

        } catch (Exception ex) {

            return fail("FAILED");

        }

    }

}
