/*
 *  Copyright 2005-2006 The Apache Software Foundation or its licensors, as applicable.
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
 * @author Roman S. Bushmanov
 * @version $Revision: 1.1.2.2.4.4 $
 */

package java.lang;

import java.util.List;
import java.util.LinkedList;
import java.util.Iterator;
import java.util.WeakHashMap;
import java.util.Set;

/**
 * @com.intel.drl.spec_ref 
 */

public class ThreadGroup implements Thread.UncaughtExceptionHandler{

    /**
     * Indent used to print information about thread group
     */
    private final static String LISTING_INDENT = "    ";

    /**
     * This group's max priority
     */
    int maxPriority = Thread.MAX_PRIORITY;

    /**
     * This group's name
     */
    String name;

    /**
     * Indicates if this thread group was marked as daemon
     */
    private boolean daemon;

    /**
     * Indicates if this thread group was already destroyed
     */
    private boolean destroyed = false;

    /**
     * List of subgroups of this thread group
     */
    private LinkedList groups = new LinkedList();

    /**
     * Parent thread group of this thread group.
     *
     */
    ThreadGroup parent;

    /**
     * All threads in the group.
     */
    private WeakHashMap threads = new WeakHashMap();

    /**
     * @com.intel.drl.spec_ref
     */
    public ThreadGroup(String name) {
        this(Thread.currentThread().group, name);
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public ThreadGroup(ThreadGroup parent, String name) {
        if (parent == null) {
            throw new NullPointerException(
                "The parent thread group specified is null!");
        }
        parent.checkAccess();
        this.name = name;
        this.parent = parent;
        this.daemon = parent.daemon;
        parent.add(this);
    }

    /**
     * This constructor is used to create main thread group
     */
    ThreadGroup() {
        this.parent = null;
        this.name = "main";
        this.daemon = false;
    }

    /**
     * @com.intel.drl.spec_ref Note: A thread is supposed to be active if and
     *                         only if it is alive.
     */
    public int activeCount() {
        int count = 0;
        List groupsListCopy = null; // a copy of subgroups list
        Set threadsCopy = null; // threads
        synchronized (this) {
            if (destroyed) {
                return 0;
            }
         	groupsListCopy = (List)groups.clone();
            threadsCopy = threads.keySet();
        }
        for (Iterator it = threadsCopy.iterator(); it.hasNext();) {
            if ( ((Thread)it.next()).isAlive()) {
                count++;
            }
        }
        for (Iterator it = groupsListCopy.iterator(); it.hasNext();) {
            count += ((ThreadGroup)it.next()).activeCount();
        }
        return count;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public int activeGroupCount() {
        int count;
        List groupsListCopy = null; // a copy of subgroups list
        synchronized (this) {
            if (destroyed) {
                return 0;
            }
            count = groups.size();
            groupsListCopy = (List)groups.clone();
        }
        for (Iterator it = groupsListCopy.iterator(); it.hasNext();) {
            count += ((ThreadGroup)it.next()).activeGroupCount();
        }
        return count;
    }

    /**
     * @com.intel.drl.spec_ref Note: This implementation always returns
     *                         <code>false</code>.
     */
    public boolean allowThreadSuspension(boolean b) {
        return false;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final void checkAccess() {
        SecurityManager securityManager = System.getSecurityManager();
        if (securityManager != null) {
            securityManager.checkAccess(this);
        }
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public synchronized final void destroy() {
        checkAccess();
        if (destroyed) {
			throw new IllegalThreadStateException(
					"The thread group " + name + " is already destroyed!");
		}
        nonsecureDestroy();
        if (parent != null) {
            parent.remove(this);
        }
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public int enumerate(Thread[] list) {
        checkAccess();
        return enumerate(list, 0, true);
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public int enumerate(Thread[] list, boolean recurse) {
        checkAccess();
        return enumerate(list, 0, recurse);
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public int enumerate(ThreadGroup[] list) {
        checkAccess();
        return enumerate(list, 0, true);
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public int enumerate(ThreadGroup[] list, boolean recurse) {
        checkAccess();
        return enumerate(list, 0, recurse);
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final int getMaxPriority() {
        return maxPriority;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final String getName() {
        return name;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final ThreadGroup getParent() {
        if (parent != null) {
            parent.checkAccess();
        }
        return parent;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final void interrupt() {
        checkAccess();
        nonsecureInterrupt();
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final boolean isDaemon() {
        return daemon;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public boolean isDestroyed() {
        return destroyed;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public void list() {
        list("");
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final boolean parentOf(ThreadGroup group) {
        ThreadGroup parent = group;
        while (parent != null) {
            if (this == parent) {
                return true;
            }
            parent = parent.parent;
        }
        return false;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final void resume() {
        checkAccess();
        nonsecureResume();
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final void setDaemon(boolean daemon) {
        checkAccess();
        this.daemon = daemon;
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final void setMaxPriority(int priority) {
        checkAccess();
        if (priority > Thread.MAX_PRIORITY || priority < Thread.MIN_PRIORITY) {
            return;
        }
        nonsecureSetMaxPriority(priority);
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final void stop() {
        checkAccess();
        nonsecureStop();
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public final void suspend() {
        checkAccess();
        nonsecureSuspend();
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public String toString() {
        return getClass().getName() + "[name=" + name + ",maxpri="
            + maxPriority + "]";
    }

    /**
     * @com.intel.drl.spec_ref
     */
    public void uncaughtException(Thread thread, Throwable throwable) {
        if(parent != null){
           parent.uncaughtException(thread, throwable);
           return;
        } 
        Thread.UncaughtExceptionHandler defaultHandler = Thread.getDefaultUncaughtExceptionHandler();
        if(defaultHandler != null){
            defaultHandler.uncaughtException(thread, throwable);
            return;
        }
        if(throwable instanceof ThreadDeath){
            return;
        }
        System.err.println("Uncaught exception in "+thread.getName()+":");
        throwable.printStackTrace();
    }

    /**
     * Adds a thread to this thread group
     */
    synchronized void add(Thread thread) {
        if (destroyed) {
            throw new IllegalThreadStateException(
                "The thread group is already destroyed!");
        }
        threads.put(thread, this);
    }

    /**
     * Removes a thread from this thread group
     */
    synchronized void remove(Thread thread) {
        if (destroyed) {
            return;
        }
        threads.remove(thread);
        thread.group = null;
        if (daemon && threads.isEmpty() && groups.isEmpty()) {
            // destroy this group
            if (parent != null) {
                parent.remove(this);
                destroyed = true;
            }
        }
    }

    /**
     * Adds a subgroup to this thread group
     */
    private synchronized void add(ThreadGroup group) {
        if (destroyed) {
            throw new IllegalThreadStateException(
                "The thread group is already destroyed!");
        }
        groups.add(group);
    }

    /**
     * Copies all the threads contained in the snapshot of this thread group to
     * the array specified starting from the specified position. <br>
     * If the specified array is not long enough to take all the threads of this
     * thread group, the exta threads are silently ignored. <br>
     * 
     * @param list an array to copy threads to
     * @param offset position in this array to start copying from
     * @param recurse indicates if the threads contained in the subgroups of
     *        this thread group should be recursively copied to the array
     *        specified
     * @return the number of threads in the array after the copying is
     *         done
     */
    private int enumerate(Thread[] list, int offset, boolean recurse) {
        if (destroyed) {
            return offset;
        }
        List groupsListCopy = null; // a copy of subgroups list
        Set threadsCopy = null; // a copy of threads list
        synchronized (this) {
            threadsCopy = threads.keySet();
            if (recurse) {
                groupsListCopy = (List)groups.clone();
            }
        }
        for (Iterator it = threadsCopy.iterator(); it.hasNext();) {
            Thread nextThread = (Thread)it.next();
            if (nextThread.isAlive()) {
                list[offset++] = nextThread;
                if (offset == list.length) {
                    return offset;
                }
            }
        }
        if (recurse) {
            for (Iterator it = groupsListCopy.iterator(); offset < list.length
                && it.hasNext();) {
                offset = ((ThreadGroup)it.next()).enumerate(list, offset, true);
            }
        }
        return offset;
    }

    /**
     * Copies all the subgroups contained in the snapshot of this thread group
     * to the array specified starting from the specified position. <br>
     * If the specified array is not long enough to take all the subgroups of
     * this thread group, the exta subgroups are silently ignored. <br>
     * 
     * @param list an array to copy subgroups to
     * @param offset position in this array to start copying from
     * @param recurse indicates if the subgroups contained in the subgroups of
     *        this thread group should be recursively copied to the array
     *        specified
     * @return the number of subgroups in the array after the copying
     *         is done
     */
    private int enumerate(ThreadGroup[] list, int offset, boolean recurse) {
        if (destroyed) {
            return offset;
        }
        int firstGroupIdx = offset;
        synchronized (this) {
            for (Iterator it = groups.iterator(); it.hasNext();) {
                list[offset++] = (ThreadGroup)it.next();
                if (offset == list.length) {
                    return offset;
                }
            }
        }
        if (recurse) {
            int lastGroupIdx = offset;
            for (int i = firstGroupIdx; offset < list.length
                && i < lastGroupIdx; i++) {
                offset = list[i].enumerate(list, offset, true);
            }
        }
        return offset;
    }

    /**
     * Recursively prints the information about this thread group using
     * <code>prefix</code> string as indent.
     */
    private void list(String prefix) {
        System.out.println(prefix + toString());
        prefix += LISTING_INDENT;
        List groupsListCopy = null; // a copy of subgroups list
        Set threadsCopy = threads.keySet();
        synchronized (this) {
            for (Iterator it = threadsCopy.iterator(); it.hasNext();) {
                System.out.println(prefix + it.next());
            }
            groupsListCopy = (List)groups.clone();
        }
        for (Iterator it = groupsListCopy.iterator(); it.hasNext();) {
            ((ThreadGroup)it.next()).list(prefix);
        }
    }

    /**
     * Destroys this thread group without any security checks. We add this
     * method to avoid calls to the checkAccess() method on subgroups
     */
    private synchronized void nonsecureDestroy() {
		if (!threads.isEmpty()) {
			throw new IllegalThreadStateException(
					"The thread group " + name + " is not empty!");
		}
		for (Iterator it = ((List)groups.clone()).iterator(); it.hasNext();) {
			((ThreadGroup) it.next()).nonsecureDestroy();
			it.remove();
		}
		destroyed = true;
	}

    /**
     * Interrupts this thread group without any security checks. We add this
     * method to avoid calls to the checkAccess() method on subgroups
     */
    private synchronized void nonsecureInterrupt() {
        for (Iterator it = threads.keySet().iterator(); it.hasNext();) {
            ((Thread)it.next()).interrupt();
        }
        for (Iterator it = groups.iterator(); it.hasNext();) {
            ((ThreadGroup)it.next()).nonsecureInterrupt();
        }
    }

    /**
     * Resumes this thread group without any security checks. We add this method
     * to avoid calls to the checkAccess() method on subgroups
     */
    private synchronized void nonsecureResume() {
        for (Iterator it = threads.keySet().iterator(); it.hasNext();) {
            ((Thread)it.next()).resume();
        }
        for (Iterator it = groups.iterator(); it.hasNext();) {
            ((ThreadGroup)it.next()).nonsecureResume();
        }
    }

    /**
     * Sets the maximum priority allowed for this thread group and its subgroups.
     * We add this method to avoid calls to the checkAccess() method on subgroups
     */
    private void nonsecureSetMaxPriority(int priority) {
        maxPriority = (parent != null && parent.maxPriority < priority)
            ? parent.maxPriority : priority;
        List groupsListCopy = null; // a copy of subgroups list
        synchronized (this) {
            groupsListCopy = (List)groups.clone();
        }
        for (Iterator it = groupsListCopy.iterator(); it.hasNext();) {
            ((ThreadGroup)it.next()).setMaxPriority(maxPriority);
        }
    }

    /**
     * Stops this thread group without any security checks.
     * We add this method to avoid calls to the checkAccess() method on subgroups
     */
    private synchronized void nonsecureStop() {
        for (Iterator it = threads.keySet().iterator(); it.hasNext();) {
            ((Thread)it.next()).stop();
        }
        for (Iterator it = groups.iterator(); it.hasNext();) {
            ((ThreadGroup)it.next()).nonsecureStop();
        }
    }

    /**
     * Suspends this thread group without any security checks.
     * We add this method to avoid calls to the checkAccess() method on subgroups
     */
    private synchronized void nonsecureSuspend() {
        for (Iterator it = threads.keySet().iterator(); it.hasNext();) {
            ((Thread)it.next()).suspend();
        }
        for (Iterator it = groups.iterator(); it.hasNext();) {
            ((ThreadGroup)it.next()).nonsecureSuspend();
        }
    }

    /**
     * Removes the specified thread group from this group.
     * 
     * @param group group to be removed from this one
     */
    private synchronized void remove(ThreadGroup group) {
    	groups.remove(group);
        if (daemon && threads.isEmpty() && groups.isEmpty()) {
        	// destroy this group
            if (parent != null) {
                parent.remove(this);
                destroyed = true;
            }
        }
    }
}
