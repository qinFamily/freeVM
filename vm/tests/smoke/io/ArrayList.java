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
 * @author Alexei Fedotov
 * @version $Revision: 1.6.8.1.4.3 $
 */  
package io;

import java.io.*;

/**
 * @keyword XXX_bug_6363
 */
public class ArrayList {
    public static void main(String[] args) throws Exception {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        ObjectOutputStream oos = new ObjectOutputStream(baos);
        oos.writeObject(new java.util.ArrayList());
        oos.close();
        System.err.println("closed");

        ObjectInputStream ois = new ObjectInputStream(new ByteArrayInputStream
(baos.toByteArray()));
        System.err.println("opened");
        
        ois.readObject();
        System.err.println("read");

        ois.close();
        System.out.println("PASS");
    }
}

