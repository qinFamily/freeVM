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
 */ 

/*
*/


package org.apache.harmony.test.func.api.java.lang.Object.notify.notify10.notify1010;

import org.apache.harmony.share.Test;


import org.apache.harmony.test.func.share.MyLog;

public class notify1010 extends Test {

    boolean  results[] = new boolean[100];
    String  logArray[] = new String[100];
    int     logIndex   = 0; 
        Object nObjects[] = { null, new Object(), new Object() };
        String   nNames[] = { null, "t1", "t2" };
        int      nCounts[] = { 0, 0 };

    void addLog(String s) {
        if ( logIndex < logArray.length )
            logArray[logIndex] = s;
        logIndex++;
    }

    void notify1010() {

            Threadnotify1010 nt1[] = { null,
                                      new Threadnotify1010(nNames[1]),
                                      new Threadnotify1010(nNames[2])
                                    };
        label: {

            synchronized(nObjects[2]) {
                for ( int i = 1; i < nt1.length; i++ ) {
                    try {
                        nt1[i].start();
                        nObjects[2].wait();
                    } catch (InterruptedException e) {
                        addLog("ERROR: unexpectead InterruptedException");
                        results[results.length -1] = false;
                        break label;
                    }
                }
            }
//-1
            try {
                nObjects[1].notify();
                results[1] = false;
            } catch (IllegalMonitorStateException e) {
            }
//-1)
//-2
            synchronized(nObjects[1]) {
                nCounts[0]++; nCounts[1]++;
                nObjects[1].notify();
                try {
                    nObjects[1].wait(60000);
                } catch (InterruptedException e) {
                    addLog("ERROR: InterruptedException " +
                                               "while nObect2.wait();" );
                    results[results.length -1] = false;
                    break label;
                }
                results[2] = ( (nCounts[0] == 1) ^ (nCounts[1] == 1) ) ;

addLog("---- nCounts[0] = "+nCounts[0] 
+" nCounts[1] = "+nCounts[1] +" res="+results[2] );

                nObjects[1].notify();
            }
            for ( int i = 1; i < nt1.length; i++ ) {
                try {
                    nt1[i].join();
                } catch (InterruptedException e) {
                    addLog("ERROR: InterruptedException " +
                                               "while nt1[i].join(); i=" +i );
                    results[results.length -1] = false;
                    break label;
                }
            }
//-2)
        } // label
        return ;
    }

class Threadnotify1010 extends Thread {
    Threadnotify1010(String s) {super(s);}
    public void run() {
        label: {
            synchronized(nObjects[1]) {
                synchronized(nObjects[2]) {
                    nObjects[2].notify();
                }
                try {
                    if ( this.getName().equals(nNames[1]) ) {
                        while ( nCounts[0] < 1 ) {
                            nObjects[1].wait();
                            nCounts[0]++;
                        }
                    }
                    if ( this.getName().equals(nNames[2]) ) {
                        while ( nCounts[1] < 1 ) {
                            nObjects[1].wait();
                            nCounts[1]++;
                        }
                    }
                } catch (InterruptedException e) {
                    addLog("ERROR: InterruptedException " +
                                       "while nObjects[1].wait(); thread=" +this.getName() );
                    results[results.length -1] = false;
                    break label;
                }
            }
        } // label
    }
}


    public int test() {

        logIndex = 0;

        String texts[] = { "Testcase FAILURE, results[#] = " ,
                           "Test P A S S E D"                ,
                           "Test F A I L E D"                ,
                           "#### U N E X P E C T E D : "     };

        int    failed   = 105;
        int    passed   = 104;
        int  unexpected = 106;

        int    toReturn = 0;
        String toPrint  = null;

        for ( int i = 0; i < results.length; i++ )
            results[i] = true;

        try {

            addLog("*********  Test notify1010 begins ");
notify1010();
            addLog("*********  Test notify1010 results: ");

            boolean result = true;
            for ( int i = 1 ; i < results.length ; i++ ) {
                result &= results[i];
                if ( ! results[i] )
                    addLog(texts[0] + i);
            }
            if ( ! result ) {
                toPrint  = texts[2];
                toReturn = failed;
            }
            if ( result ) {
                toPrint  = texts[1];
                toReturn = passed;
            }
        } catch (Exception e) {
            toPrint  = texts[3] + e;
            toReturn = unexpected;
        }
        if ( toReturn != passed )
            for ( int i = 0; i < logIndex; i++ )
                MyLog.toMyLog(logArray[i]);

        MyLog.toMyLog(toPrint);
        return toReturn;
    }

    public static void main(String args[]) {
        System.exit(new notify1010().test());
    }
}



