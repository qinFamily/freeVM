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
package org.apache.harmony.nio.internal;

import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InterruptedIOException;
import java.net.ConnectException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.DatagramSocketImpl;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.channels.AlreadyConnectedException;
import java.nio.channels.ClosedChannelException;
import java.nio.channels.DatagramChannel;
import java.nio.channels.NotYetConnectedException;
import java.nio.channels.spi.SelectorProvider;

import org.apache.harmony.luni.net.NetUtil;
import org.apache.harmony.luni.net.SocketImplProvider;
import org.apache.harmony.luni.platform.FileDescriptorHandler;
import org.apache.harmony.luni.platform.INetworkSystem;
import org.apache.harmony.luni.platform.Platform;



/*
 * The default implementation class of java.nio.channels.DatagramChannel.
 * 
 */
class DatagramChannelImpl extends DatagramChannel implements FileDescriptorHandler{

    // -------------------------------------------------------------------
    // Class variables
    // -------------------------------------------------------------------

    // The singleton to do the native network operation.
    private static final INetworkSystem networkSystem = Platform
            .getNetworkSystem();

    // default timeout used to nonblocking mode.
    private static final int DEFAULT_TIMEOUT = 1;

    // error messages, for native dependent.
    private static final String ERRMSG_TIMEOUT = "The operation timed out";


    // -------------------------------------------------------------------
    // Instance variables
    // -------------------------------------------------------------------

    // The fd to interact with native code
    private FileDescriptor fd;

    // Our internal DatagramSocket.
    private DatagramSocket socket = null;

    // The address to be connected.
    InetSocketAddress connectAddress = null;

    // local port
    private int localPort;

    // At first, uninitialized.
    boolean connected = false;

    // whether the socket is bound
    boolean isBound = false;

    // lock for read and receive
    private final Object readLock = new Object();

    // lock for write and send
    private final Object writeLock = new Object();

    // used to store the trafficClass value which is simply returned
    // as the value that was set. We also need it to pass it to methods
    // that specify an address packets are going to be sent to
    private int trafficClass = 0;

    // -------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------

    /*
     * Constructor
     */
    protected DatagramChannelImpl(SelectorProvider selectorProvider)
            throws IOException {
        super(selectorProvider);
        fd = new FileDescriptor();
        networkSystem.createDatagramSocket(fd, true);
    }

    // -------------------------------------------------------------------
    // Methods for getting internal DatagramSocket.
    // -------------------------------------------------------------------

    /*
     * Getting the internal DatagramSocket If we have not the socket, we create
     * a new one.
     */
    synchronized public DatagramSocket socket() {
        if (null == socket) {
            socket = new DatagramSocketAdapter(SocketImplProvider
                    .getDatagramSocketImpl(fd, localPort), this);
        }
        return socket;
    }
    
    /**
     * Answer the local address from the IP stack. This method should not be
     * called directly as it does not check the security policy.
     * 
     * @return InetAddress the local address to which the socket is bound.
     * @see DatagramSocket
     */
    InetAddress getLocalAddress() {
        return networkSystem.getSocketLocalAddress(fd, NetUtil.preferIPv6Addresses());
    }

    // -------------------------------------------------------------------
    // Methods for connect and disconnect
    // -------------------------------------------------------------------

    /*
     * 
     * @see java.nio.channels.DatagramChannel#isConnected()
     */
    synchronized public boolean isConnected() {
        return connected;
    }

    /*
     * 
     * @see java.nio.channels.DatagramChannel#connect(java.net.SocketAddress)
     */
    synchronized public DatagramChannel connect(SocketAddress address)
            throws IOException {
        // must open
        checkOpen();
        // status must be un-connected.
        if (connected) {
            throw new IllegalStateException();
        }

        // check the address
        InetSocketAddress inetSocketAddress = SocketChannelImpl
                .validateAddress(address);

        // security check
        SecurityManager sm = System.getSecurityManager();
        if (null != sm) {
            if (inetSocketAddress.getAddress().isMulticastAddress()) {
                sm.checkMulticast(inetSocketAddress.getAddress());
            } else {
                sm.checkConnect(inetSocketAddress.getAddress().getHostName(),
                        inetSocketAddress.getPort());
            }
        }

        try {
            begin();
            networkSystem.connectDatagram(fd, inetSocketAddress.getPort(),
                    trafficClass, inetSocketAddress.getAddress());
        } catch (ConnectException e) {
            // ConnectException means connect fail, not exception
        } finally {
            end(true);
        }

        // set the connected address.
        connectAddress = inetSocketAddress;
        connected = true;
        isBound = true;
        return this;
    }

    /*
     * 
     * @see java.nio.channels.DatagramChannel#disconnect()
     */
    synchronized public DatagramChannel disconnect() throws IOException {
        if (!isConnected() || !isOpen()) {
            return this;
        }
        connected = false;
        connectAddress = null;
        networkSystem.disconnectDatagram(fd);
        if (null != socket) {
            socket.disconnect();
        }
        return this;
    }

    // -------------------------------------------------------------------
    // Methods for send and receive
    // -------------------------------------------------------------------

    /*
     * 
     * @see java.nio.channels.DatagramChannel#receive(java.nio.ByteBuffer)
     */
    public SocketAddress receive(ByteBuffer target) throws IOException {
        // must not null and not readonly
        checkNotNullNotReadOnly(target);
        // must open
        checkOpen();

        if (!isBound) {
            return null;
        }

        SocketAddress retAddr = null;
        try {
            begin();

            // receive real data packet, (not peek)
            synchronized (readLock) {
                boolean loop = isBlocking();
                do {
                    DatagramPacket receivePacket = new DatagramPacket(
                            new byte[target.remaining()], target.remaining());

                    if (isConnected()) {
                        networkSystem.recvConnectedDatagram(fd, receivePacket,
                                receivePacket.getData(), receivePacket
                                        .getOffset(),
                                receivePacket.getLength(), isBlocking()?0:DEFAULT_TIMEOUT, false);
                    } else {
                        networkSystem.receiveDatagram(fd, receivePacket,
                                receivePacket.getData(), receivePacket
                                        .getOffset(),
                                receivePacket.getLength(), isBlocking()?0:DEFAULT_TIMEOUT, false);
                    }

                    // security check
                    SecurityManager sm = System.getSecurityManager();
                    if (!isConnected() && null != sm) {
                        try {
                            sm.checkAccept(receivePacket.getAddress()
                                    .getHostAddress(), receivePacket.getPort());
                        } catch (SecurityException e) {
                            // do discard the datagram packet
                            receivePacket = null;
                        }
                    }
                    if (null != receivePacket
                            && null != receivePacket.getAddress()) {
                        // copy the data of received packet
                        target.put(ByteBuffer.wrap(receivePacket.getData()));
                        retAddr = receivePacket.getSocketAddress();
                        break;
                    }
                } while (loop);
            }
        } catch (InterruptedIOException e) {
            // this line used in Linux
            return null;
        } finally {
            end(null != retAddr);
        }
        return retAddr;
    }

    /*
     * @see java.nio.channels.DatagramChannel#send(java.nio.ByteBuffer,
     *      java.net.SocketAddress)
     */
    public int send(ByteBuffer source, SocketAddress address)
            throws IOException {
        // must not null
        checkNotNull(source);
        // must open
        checkOpen();

        // transfer address
        InetSocketAddress isa = (InetSocketAddress) address;
        if (null == isa.getAddress()) {
            throw new IOException();
        }

        if (isConnected()) {
            if (!connectAddress.equals(isa)) {
                throw new IllegalArgumentException();
            }
        } else {
            // not connected, check security
            SecurityManager sm = System.getSecurityManager();
            if (sm != null) {
                if (isa.getAddress().isMulticastAddress()) {
                    sm.checkMulticast(isa.getAddress());
                } else {
                    sm.checkConnect(isa.getAddress().getHostAddress(), isa
                            .getPort());
                }
            }
        }

        // the return value.
        int sendCount = 0;

        try {
            begin();
            byte[] array;
            int length = source.remaining();
            int oldposition = source.position();
            int start;
            if (source.hasArray()) {
                array = source.array();
                start = oldposition;
            } else {
                array = new byte[length];
                source.get(array);
                start = 0;
            }            
            synchronized (writeLock) {
                sendCount = networkSystem.sendDatagram(fd, array, start,
                        length, isa.getPort(), false, trafficClass, isa
                                .getAddress());
            }
            source.position(oldposition + sendCount);            
            return sendCount;
        } finally {
            end(sendCount >= 0);
        }
    }

    // -------------------------------------------------------------------
    // Methods for read and write.
    // -------------------------------------------------------------------

    /*
     * 
     * @see java.nio.channels.DatagramChannel#read(java.nio.ByteBuffer)
     */
    public int read(ByteBuffer target) throws IOException {
        if (null == target){
            throw new NullPointerException();
        }
        // status must be open and connected
        checkOpenConnected();
        // target buffer must be not null and not readonly
        checkNotNullNotReadOnly(target);

        if (!target.hasRemaining()){
            return 0;
        }        
        byte[] readArray = new byte[target.remaining()];
        int readCount;
        synchronized (readLock) {
            readCount = readImpl(readArray);
        }
        if (0 != readCount) {
            target.put(readArray, 0, readCount);
        }
        return readCount;
    }

    /*
     * 
     * @see java.nio.channels.DatagramChannel#read(java.nio.ByteBuffer[], int,
     *      int)
     */
    public long read(ByteBuffer[] targets, int offset, int length)
            throws IOException {
        if (length < 0 || offset < 0 || (long)length + (long)offset > targets.length) {
            throw new IndexOutOfBoundsException();
        }        
        // status must be open and connected
        checkOpenConnected();
        
        int totalCount = 0;
        for (int val = offset; val < length; val++) {
            // target buffer must be not null and not readonly
            checkNotNullNotReadOnly(targets[val]);
            totalCount += targets[val].remaining();
        }
        // read data to readBuffer, and then transfer data from readBuffer to
        // targets.
        byte[] readBuffer = new byte[totalCount];
        int readCount ;
        synchronized (readLock) {
            readCount = readImpl(readBuffer);
        }
        if (readCount > 0) {
            int left = readCount;
            int index = offset;
            // transfer data from readBuffer to targets
            while (left > 0) {
                int putLength = Math.min(targets[index].remaining(), left);                
                targets[index].put(readBuffer, readCount - left, putLength);
                index++;
                left -= putLength;
            }
        }
        return readCount;
    }

    /*
     * read from channel, and store the result in the target.
     */
    private int readImpl(byte[] target) throws IOException {
        // the return value
        int readCount = 0;

        try {
            begin();
            // timeout == 0 means block read.
            // DEFAULT_TIMEOUT is used in non-block mode.
            int timeout = isBlocking() ? 0 : DEFAULT_TIMEOUT;
            DatagramPacket pack;
            pack = new DatagramPacket(target, target.length);
            if (isConnected()) {
                readCount = networkSystem.recvConnectedDatagram(fd, pack, pack
                        .getData(), 0, pack.getLength(), timeout, false);
            } else {
                readCount = networkSystem.receiveDatagram(fd, pack, pack
                        .getData(), 0, pack.getLength(), timeout, false);
            }
            return readCount;
        } catch (InterruptedIOException e) {
            // FIXME improve native code.
            if (e.getMessage().equals(ERRMSG_TIMEOUT)) {
                return 0;
            }
            throw e;
        } finally {
            end(readCount > 0);
        }
    }

    /*
     * @see java.nio.channels.DatagramChannel#write(java.nio.ByteBuffer)
     */
    public int write(ByteBuffer source) throws IOException {
        // source buffer must be not null
        checkNotNull(source);
        // status must be open and connected
        checkOpenConnected();
        // return immediately if source is full
        if (!source.hasRemaining()){
            return 0;
        }

        synchronized (writeLock) {
            return writeImpl(source);
        }
    }

    /*
     * @see java.nio.channels.DatagramChannel#write(java.nio.ByteBuffer[], int,
     *      int)
     */
    public long write(ByteBuffer[] sources, int offset, int length)
            throws IOException {
        if (length < 0 || offset < 0 || (long)length + (long)offset > sources.length) {
            throw new IndexOutOfBoundsException();
        }
        // status must be open and connected
        checkOpenConnected();
        synchronized (writeLock) {
            int count = 0;
            for (int val = offset; val < length; val++) {
                count = count + sources[val].remaining();
            }
            ByteBuffer writeBuf = ByteBuffer.allocate(count);
            for (int val = offset; val < length; val++) {
                writeBuf.put(sources[val]);
            }
            writeBuf.flip();
            return writeImpl(writeBuf);
        }
    }

    /*
     * wirte the source. return the count of bytes written.
     */
    private int writeImpl(ByteBuffer buf) throws IOException {
        // the return value
        int result = 0;
        try {
            begin();            
            byte[] array;
            int length = buf.remaining();
            int oldposition = buf.position();
            int start;
            if (buf.hasArray()) {
                array = buf.array();  
                start = oldposition;
            } else {
                array = new byte[length];
                buf.get(array);
                start = 0;
            }
            result = networkSystem.sendConnectedDatagram(fd, array, start,
                    length, isBound);
            buf.position(oldposition + result);
            return result;
        } finally {
            end(result > 0);
        }
    }

    // -------------------------------------------------------------------
    // Protected Inherited methods
    // -------------------------------------------------------------------

    /*
     * do really closing action here
     */
    synchronized protected void implCloseSelectableChannel() throws IOException {
        connected = false;
        if (null != socket && !socket.isClosed()) {
            socket.close();
        } else {
            networkSystem.socketClose(fd);
        }
    }

    /*
     * 
     * @see java.nio.channels.spi.AbstractSelectableChannel#implConfigureBlocking(boolean)
     */
    protected void implConfigureBlocking(boolean blockingMode)
            throws IOException {
        // Do nothing here. For real read/write operation in nonblocking mode,
        // it uses select system call. Whether a channel is blocking can be
        // decided by isBlocking() method.
    }

    // -------------------------------------------------------------------
    // Share methods for checking.
    // -------------------------------------------------------------------

    /*
     * status check, must be open.
     */
    private void checkOpen() throws IOException {
        if (!isOpen()) {
            throw new ClosedChannelException();
        }
    }

    /*
     * status check, must be open and connected, for read and write.
     */
    private void checkOpenConnected() throws IOException {
        checkOpen();
        if (!isConnected()) {
            throw new NotYetConnectedException();
        }
    }

    /*
     * buffer check, must not null
     */
    private void checkNotNull(ByteBuffer source) {
        if (null == source) {
            throw new NullPointerException();
        }
    }

    /*
     * buffer check, must not null and not read only buffer, for read and
     * receive.
     */
    private void checkNotNullNotReadOnly(ByteBuffer target) {
        // including checking of NPE.
        if (target.isReadOnly()) {
            throw new IllegalArgumentException();
        }
    }

    // -------------------------------------------------------------------
    // Adapter classes for internal socket.
    // -------------------------------------------------------------------

    /*
     * get the fd for internal use.
     */
    public FileDescriptor getFD() {
        return fd;
    }

    /*
     * The adapter class of DatagramSocket
     */
    private static class DatagramSocketAdapter extends DatagramSocket {

        /*
         * The internal datagramChannelImpl.
         */
        private DatagramChannelImpl channelImpl;
        
        /*
         * init the datagramSocketImpl and datagramChannelImpl
         */
        DatagramSocketAdapter(DatagramSocketImpl socketimpl,
                DatagramChannelImpl channelImpl) {
            super(socketimpl);
            this.channelImpl = channelImpl;            
        }

        /*
         * get the internal datagramChannelImpl
         */
        public DatagramChannel getChannel() {
            return channelImpl;
        }

        /*
         * @see java.net.DatagramSocket#isBound()
         */
        public boolean isBound() {
            return channelImpl.isBound;
        }

        /*
         * @see java.net.DatagramSocket#isConnected()
         */
        public boolean isConnected() {
            return channelImpl.isConnected();
        }

        /*
         * @see java.net.DatagramSocket#getInetAddress()
         */
        public InetAddress getInetAddress() {
            if (null == channelImpl.connectAddress) {
                return null;
            }
            return channelImpl.connectAddress.getAddress();
        }
        
        /*
         * @see java.net.DatagramSocket#getLocalAddress()
         */
        public InetAddress getLocalAddress(){
            return channelImpl.getLocalAddress();
        }

        /*
         * @see java.net.DatagramSocket#getPort()
         */
        public int getPort() {
            if (null == channelImpl.connectAddress) {
                return -1;
            }
            return channelImpl.connectAddress.getPort();
        }

        /*
         * @see java.net.DatagramSocket#bind(java.net.SocketAddress)
         */
        public void bind(SocketAddress localAddr) throws SocketException {
            if (channelImpl.isConnected()) {
                throw new AlreadyConnectedException();
            }
            super.bind(localAddr);
            channelImpl.isBound = true;
        }

        /*
         * @see java.net.DatagramSocket#close()
         */
        public void close() {
            synchronized (channelImpl) {
                if (channelImpl.isOpen()) {
                    try {
                        channelImpl.close();
                    } catch (IOException e) {
                        // Ignore
                    }
                }
                super.close();
            }
        }

        /*
         * @see java.net.DatagramSocket#disconnect()
         */
        public void disconnect() {
            try {
                channelImpl.disconnect();
            } catch (IOException e) {
                // Ignore
            }
            super.disconnect();
        }
    }
}
