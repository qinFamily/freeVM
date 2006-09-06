/*
 *  Copyright 2005 - 2006 The Apache Software Software Foundation or its licensors, as applicable.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
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
 * @author Michael Danilov, Pavel Dolgov
 * @version $Revision$
 */
package java.awt;

import org.apache.harmony.awt.wtk.NativeEvent;
import org.apache.harmony.awt.wtk.NativeEventQueue;

final class EventDispatchThread extends Thread  {
    
    private static final class MarkerEvent extends AWTEvent {
        MarkerEvent(Object source, int id) {
            super(source, id);
        }
    }

    final Dispatcher dispatcher;
    final Toolkit toolkit;
    private NativeEventQueue nativeQueue;

    private volatile boolean shutdownPending = false;

    /**
     * Initialise and run the main event loop
     */
    public void run() {
        nativeQueue = toolkit.getNativeEventQueue();

        try {
            runModalLoop(null);
        } catch (Exception e) {
            e.printStackTrace();
        }

        toolkit.shutdownWatchdog.forceShutdown();
    }

    void runModalLoop(ModalContext context) {
        long lastPaintTime = System.currentTimeMillis();
        while (!shutdownPending && (context == null || context.isModalLoopRunning())) {
            EventQueue eventQueue = toolkit.getSystemEventQueueImpl();

            NativeEvent ne = nativeQueue.getNextEvent();
            if (ne != null) {
                dispatcher.onEvent(ne);
                MarkerEvent marker = new MarkerEvent(this, 0);
                eventQueue.postEvent(marker);
                for (AWTEvent ae = eventQueue.getNextEventNoWait(); 
                        (ae != null) && (ae != marker); 
                        ae = eventQueue.getNextEventNoWait()) {
                    eventQueue.dispatchEvent(ae);
                }
            } else {
                toolkit.shutdownWatchdog.setNativeQueueEmpty(true);
                AWTEvent ae = eventQueue.getNextEventNoWait();
                if (ae != null) {
                    eventQueue.dispatchEvent(ae);
                    long curTime = System.currentTimeMillis();
                    if (curTime - lastPaintTime > 10) {
                        toolkit.onQueueEmpty();
                        lastPaintTime = System.currentTimeMillis();
                    }
                } else {
                    toolkit.shutdownWatchdog.setAwtQueueEmpty(true);
                    toolkit.onQueueEmpty();
                    lastPaintTime = System.currentTimeMillis();
                    waitForAnyEvent();
                }
            }
        }
    }
    
    private void waitForAnyEvent() {
        EventQueue eventQueue = toolkit.getSystemEventQueueImpl();
        if (!eventQueue.isEmpty() || !nativeQueue.isEmpty()) {
            return;
        }
        Object eventMonitor = nativeQueue.getEventMonitor();
        synchronized(eventMonitor) {
            try {
                eventMonitor.wait();
            } catch (InterruptedException e) {}
        }
    }

    void shutdown() {
        shutdownPending = true;
    }

    EventDispatchThread(Toolkit toolkit, Dispatcher dispatcher ) {
        this.toolkit = toolkit;
        this.dispatcher = dispatcher;
        setName("AWT-EventDispatchThread");
        setDaemon(true);
    }

}
