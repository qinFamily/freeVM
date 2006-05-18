/*
 * Copyright 2005-2006 The Apache Software Foundation or its licensors, as applicable
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
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @author  Mikhail A. Markov
 * @version $Revision: 1.7.4.3 $
 */
package java.rmi.server;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import org.apache.harmony.rmi.transport.DefaultRMISocketFactory;


/**
 * @com.intel.drl.spec_ref
 *
 * @author  Mikhail A. Markov
 * @version $Revision: 1.7.4.3 $
 */
public abstract class RMISocketFactory
        implements RMIClientSocketFactory, RMIServerSocketFactory {

    /*
     * Global RMISocketFactory set by user application and null
     * if default RMISocketFactory is used.
     */
    private static RMISocketFactory globalRsf = null;

    /*
     * Global RMIFailureHandler set by user application.
     */
    private static RMIFailureHandler globalRfh = null;

    /*
     * Default RMISocketFactory.
     */
    private static RMISocketFactory defaultRsf = null;

    /**
     * @com.intel.drl.spec_ref
     */
    public RMISocketFactory() {
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public abstract Socket createSocket(String host, int port)
            throws IOException;

    /**
     * @com.intel.drl.spec_ref
     */
    public abstract ServerSocket createServerSocket(int port)
            throws IOException;

    /**
     * @com.intel.drl.spec_ref
     */
    public static synchronized void setSocketFactory(RMISocketFactory rsf)
            throws IOException, SecurityException {
        SecurityManager mgr = System.getSecurityManager();

        if (mgr != null) {
            mgr.checkSetFactory();
        }

        if (globalRsf == null) {
            throw new IOException("RMISocketFactory has been already set.");
        }
        globalRsf = rsf;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public static synchronized RMISocketFactory getSocketFactory() {
        return globalRsf;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public static synchronized RMISocketFactory getDefaultSocketFactory() {
        if (defaultRsf == null) {
            defaultRsf = new DefaultRMISocketFactory();
        }
        return defaultRsf;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public static synchronized void setFailureHandler(RMIFailureHandler rfh)
            throws SecurityException {
        SecurityManager mgr = System.getSecurityManager();

        if (mgr != null) {
            mgr.checkSetFactory();
        }
        globalRfh = rfh;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public static synchronized RMIFailureHandler getFailureHandler() {
        return globalRfh;
    }
}
