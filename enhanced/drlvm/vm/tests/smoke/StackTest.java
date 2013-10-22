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
 * Excluded from acceptance test set on EM64T.
 *
 * @keyword X_em64t_bug
 */

public class StackTest {

    static int depth = 0;
    static final int max_depth = 10000000;

    public static void func() {
        depth++;
	if (depth < max_depth)
            func();
    }

    public static void main(String[] args) {
        try {
            func();
        } catch (StackOverflowError soe) {
            System.out.println("PASS : First SOE depth = " + depth + " : " + soe);
            return;
        }
        System.out.println("PASS: no SOE in " + max_depth + " iterations");
    }
}