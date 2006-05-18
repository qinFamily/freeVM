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
 * @author  Mikhail A. Markov, Vasily Zakharov
 * @version $Revision: 1.1.2.1 $
 */
package org.apache.harmony.rmi.transport.proxy;

import java.io.IOException;

import org.apache.harmony.rmi.common.RMIConstants;
import org.apache.harmony.rmi.transport.SocketWrapper;


/**
 * Outbound HTTP socket wrapper.
 *
 * @author  Mikhail A. Markov, Vasily Zakharov
 * @version $Revision: 1.1.2.1 $
 */
public class HttpOutboundSocket extends SocketWrapper
        implements ProxyConstants {

    /**
     * Host to connect to.
     */
    private String host;

    /**
     * Port to connect to.
     */
    private int port;

    /**
     * If this is a CGI socket.
     */
    private boolean cgi;

    /**
     * Returns new <code>HttpOutboundSocket</code> instance connected
     * to specified host and port, probably through a proxy (if proxy is set).
     * If proxy is not set, and direct HTTP connections are enabled,
     * then direct connection is established.
     *
     * Equivalent to
     * {@link #HttpOutboundSocket(Proxy, String, int, boolean)
     * HttpOutboundSocket(new Proxy(), host, port, false)}.
     *
     * @param   host
     *          Host to connect to.
     *
     * @param   port
     *          Port to connect to.
     *
     * @throws  IOException
     *          If I/O error occurs.
     */
    public HttpOutboundSocket(String host, int port) throws IOException {
        this(new Proxy(), host, port, false);
    }

    /**
     * Returns new <code>HttpOutboundSocket</code> instance connected
     * to specified host and port, probably through a proxy (if proxy is set).
     * If proxy is not set, and direct HTTP connections are enabled,
     * then direct connection is established.
     *
     * Equivalent to
     * {@link #HttpOutboundSocket(Proxy, String, int, boolean)
     * HttpOutboundSocket(proxy, host, port, false)}.
     *
     * @param   proxy
     *          Proxy configuration.
     *
     * @param   host
     *          Host to connect to.
     *
     * @param   port
     *          Port to connect to.
     *
     * @throws  IOException
     *          If I/O error occurs.
     */
    public HttpOutboundSocket(Proxy proxy, String host, int port)
            throws IOException {
        this(proxy, host, port, false);
    }

    /**
     * Returns new <code>HttpOutboundSocket</code> instance connected
     * to specified host and port, probably through a proxy (if proxy is set).
     * If proxy is not set, and direct HTTP connections are enabled,
     * then direct connection is established.
     *
     * @param   proxy
     *          Proxy configuration.
     *
     * @param   host
     *          Host to connect to.
     *
     * @param   port
     *          Port to connect to.
     *
     * @param   cgi
     *          If this is CGI stream.
     *
     * @throws  IOException
     *          If I/O error occurs.
     */
    public HttpOutboundSocket(Proxy proxy,
            String host, int port, boolean cgi) throws IOException {
        super(proxy.getSocket(host, (cgi ? RMIConstants.HTTP_DEFAULT_PORT : port)));
        this.host = host;
        this.port = port;
        this.cgi = cgi;
        in = new HttpInputStream(in, false);
        out = new HttpOutputStream(out, false, host, port, cgi);
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return ("HttpOutboundSocket[" + s.toString() + ", "
                + host + ':' + port + ", " + (cgi ? "" : "non-") + "CGI]");
    }
}
