/* Copyright 2005, 2006 The Apache Software Foundation or its licensors, as applicable
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
package org.apache.harmony.tests.java.nio.channels;

import java.io.IOException;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.nio.channels.spi.SelectorProvider;

class MockSocketChannel extends SocketChannel {

    protected MockSocketChannel(SelectorProvider arg0) {
        super(arg0);
    }

    public Socket socket() {
        return null;
    }

    public boolean isConnected() {
        return false;
    }

    public boolean isConnectionPending() {
        return false;
    }

    public boolean connect(SocketAddress arg0) throws IOException {
        return false;
    }

    public boolean finishConnect() throws IOException {
        return false;
    }

    public int read(ByteBuffer arg0) throws IOException {
        return 0;
    }

    public long read(ByteBuffer[] arg0, int arg1, int arg2) throws IOException {
        return 0;
    }

    public int write(ByteBuffer arg0) throws IOException {
        return 0;
    }

    public long write(ByteBuffer[] arg0, int arg1, int arg2) throws IOException {
        return 0;
    }

    protected void implCloseSelectableChannel() throws IOException {
    }

    protected void implConfigureBlocking(boolean arg0) throws IOException {
    }

}
