/* Copyright 2006 The Apache Software Foundation or its licensors, as applicable
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

package java.lang.management;

/**
 * <p>
 * The management interface for a memory manager system.
 * </p>
 * <p>
 * <code>ObjectName</code> pattern: java.lang:type=MemoryManager,name=<i>manager_name</i>
 * </p>
 * 
 * @since 1.5
 */
public interface MemoryManagerMXBean {
    /**
     * <p>
     * The names of the memory pools being managed by this manager.
     * </p>
     * 
     * @return A <code>String[]</code> of pool names.
     */
    String[] getMemoryPoolNames();

    /**
     * <p>
     * The name of the memory manager.
     * </p>
     * 
     * @return The manager's name.
     */
    String getName();

    /**
     * <p>
     * Indicates whether or not the manager is currently valid. A memory manager
     * may be removed by a JVM and become invalid.
     * </p>
     * 
     * @return <code>true</code> if the manager is valid, <code>false</code>
     *         otherwise.
     */
    boolean isValid();
}
