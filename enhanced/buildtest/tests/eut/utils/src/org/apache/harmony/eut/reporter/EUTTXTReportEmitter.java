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
package org.apache.harmony.eut.reporter;
import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.PrintWriter;

/**
 * Emits the summary and issues information in txt format. 
 */
final class EUTTXTReportEmitter {

    /** 
     * Holds the maximum number of unexpected errors/failure to be included to
     * report.
     */
    private static int MAX_ERROR_FAILURE_NUMBER = 10;

    /** Keeps the output stream to write the report.txt data into. */
    private static PrintWriter out;

    /** Emits EUT issues information into specified output. */
    static void emitTXTReport(PrintWriter out, EUTSummaryInfo esi)
            throws Exception {
        EUTTXTReportEmitter.out = out;
        out.println("================================================");
        out.println("Eclipse Unit Tests " + esi.eut_version + " on " 
                + esi.os + " " + esi.ws + " " + esi.arch);
        out.println("================================================");
        out.println("Tested JRE information:");
        addFileToOutput(esi.testedJavaVersionLog);
        out.println();
        out.println("JRE options: " + esi.tested_vm_ags);
        out.println("================================================");
        out.println("Runner JRE information:");
        addFileToOutput(esi.runningJavaVersionLog);
        out.println("================================================");

        // print the summary results
        out.println();
        out.println("Total run tests     : " + esi.tests_run_total);
        out.println("Relative passrate   : " + EUTReporter.makePassrateString(
                    esi.ss.tests_reported_passed, esi.tests_run_total));
        out.println("Unexpected crashes  : "
                + esi.suites_unexpected_crashed.size());
        out.println("Unexpected errors   : "
                + esi.ss.tests_unexpected_end_with_error);
        out.println("Unexpected failures : "
                + esi.ss.tests_unexpected_end_with_failure);

        // print detailed reports
        emitCrashesResults(esi);
        emitErrorFailureResults(esi.ss.tests_unexpected_end_with_error,
                EUTTestInfo.TEST_ERROR);
        emitErrorFailureResults(esi.ss.tests_unexpected_end_with_failure,
                EUTTestInfo.TEST_FAILURE);
    }

    private static void addFileToOutput(File log) throws Exception {
        BufferedReader in = new BufferedReader(new FileReader(log));
        String line = in.readLine();

        while (line != null) {
            out.println(line);
            line = in.readLine();
        }
        in.close();
    }

    private static void emitCrashesResults(EUTSummaryInfo esi) {
        if (esi.suites_unexpected_crashed.size() == 0) {
            return;
        }
        out.println();
        out.println("================================================");
        out.println("Unexpected crashes  : "
                + esi.suites_unexpected_crashed.size());

        for (int i = 0; i < esi.suites_unexpected_crashed.size(); i++) {
            EUTSuiteInfo si = esi.suites_unexpected_crashed.get(i);
            out.println(si.name + "(" + si.tests_total + ")");
        }
    }

    private static void emitErrorFailureResults(int issuesNum,
            int issuesType) {
        if (issuesNum == 0) {
            return;
        }
        out.println();
        out.println("================================================");

        if (issuesType == EUTTestInfo.TEST_ERROR) {
            out.println("Unexpected errors   : " + issuesNum);
        } else {
            out.println("Unexpected failures : " + issuesNum);
        }
        int reported_issues_num = 0;

EMIT_ERROR_FAILURE_RESULTS:
        for (int i = 0; i < EUTReporter.suiteList.size(); i++) {
            EUTSuiteInfo si = EUTReporter.suiteList.get(i);

            if (!si.wasRun || si.isCrashed) {
                continue;
            }

            if (si.suiteIssueContent != null &&
                    issuesType == EUTTestInfo.TEST_ERROR) {
                String testsStr = si.tests_total == 1 ? " test)" : " tests)";
                out.println();
                out.println(si.name + " (" + si.tests_total + testsStr);
                out.println(si.suiteIssueMessage);
                out.println(si.suiteIssueContent.toString().trim());
            }

            for (int j = 0; j < si.unexpectedErrorFailureTests.size(); j++) {
                EUTTestInfo ti = si.unexpectedErrorFailureTests.get(j);

                if (ti.testStatus != issuesType) {
                    continue;
                }
                out.println();
                out.println(ti.testClass + '.' + ti.testName);
                out.println(ti.testIssueMessage);
                out.println(ti.testIssueContent.toString().trim());
                reported_issues_num++;

                /*
                 * The resulted report must not be too big, so one needs to
                 * limit a number of reported issues...
                 */
                if (reported_issues_num == MAX_ERROR_FAILURE_NUMBER) {
                    if (reported_issues_num !=
                            si.unexpectedErrorFailureTests.size() - 1) {
                        out.println();
                        out.println("...Skipped the rest of reports...");
                    }
                    break EMIT_ERROR_FAILURE_RESULTS;
                }
            }
        }
    }
} // end of class 'EUTTXTReportEmitter' definition
