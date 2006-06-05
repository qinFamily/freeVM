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
 * @version $Revision: 1.7.20.2.4.4 $
 */  
package outofmemory;

/**
 * Out of memory tests have no dependecy on class library.
 * @keyword slow
 */
public class Plain {
    public static final int N = 10;
    public static void main (String[] args) {
        try {
            int size = 1024;
            java.util.ArrayList a = new java.util.ArrayList(1024);
            while (size > 128) {
                try {
                    byte b[] = new byte[size];
                    b[0] = 1;
                    b[b.length-1] = 2;
                    a.add(b);
                    size *= 2;
                    System.out.print("+"); System.out.flush();
                } catch (OutOfMemoryError e) {
                    try {
                        size /= 2;
                        System.out.print("-"); System.out.flush();
                    } catch (OutOfMemoryError f) {
                    }
                }
            }

            a = null;
            System.out.println(); System.out.flush();
            System.out.println("\nPASS"); System.out.flush();

        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    double x;
    int y;
    String z;
}
