/* Copyright 2004 The Apache Software Foundation or its licensors, as applicable
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package java.util.logging;

/**
 * <p>A Filter provides a mechanism for exercising fine-grained control over
 * what records get logged.</p> 
 */
public interface Filter {

    /**
     * <p>Checks the {@link LogRecord} to determine if it should be logged.</p>
     * 
     * @param record The {@link LogRecord} to be checked.
     * @return <code>true</code> if the supplied log record needs to be
     *         logged, otherwise <code>false</code>
     */
    boolean isLoggable(LogRecord record);
}
