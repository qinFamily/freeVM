/*
 *  Copyright 2005-2006 The Apache Software Foundation or its licensors, as applicable.
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
/** 
 * @author Salikh Zakirov
 * @version $Revision: 1.6.28.4 $
 */  

package gc;

import java.lang.ref.PhantomReference;
import java.lang.ref.Reference;
import java.lang.ref.ReferenceQueue;


/**
 * @keyword ref
 */
public class PhantomReferenceTest {

    public static void main(String[] args) throws Exception {
        ReferenceQueue queue = new ReferenceQueue();
        Object referent = new PhantomReferenceTest();
        Reference ref = new PhantomReference(referent, queue);

        // drop strong reference
        referent = null;

        System.gc();
        System.out.println("waiting for a reference..");
        Reference enqueued = queue.poll();
        if (ref.get() != null) {
            System.out.println("FAIL: reference was not cleared.");
            return;
        }
        if (enqueued == null) {
            System.out.println("FAIL: reference was not enqueued");
            return;
        }
        if (enqueued.get() != null) {
            System.out.println("FAIL: reference was not cleared.");
            return;
        }
        System.out.println("PASS");
    }
}
