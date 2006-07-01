/* Copyright 1998, 2006 The Apache Software Foundation or its licensors, as applicable
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

package org.apache.harmony.luni.internal.net.www.protocol.https;

import java.io.IOException;
import java.net.Proxy;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLStreamHandler;

/**
 * Handler for HttpsURLConnection implementation.
 */
public class Handler extends URLStreamHandler {

    protected URLConnection openConnection(URL url) throws IOException {
        return new HttpsURLConnection(url, getDefaultPort());
    }

    protected URLConnection openConnection(URL url, Proxy proxy)
                                                    throws IOException {
        if((url == null) || (proxy == null)) {
            throw new IllegalArgumentException(
                    "Some of the parameters is null");
        }
        return new HttpsURLConnection(url, getDefaultPort(), proxy);
    }

    protected int getDefaultPort() {
        return 443;
    }
}

