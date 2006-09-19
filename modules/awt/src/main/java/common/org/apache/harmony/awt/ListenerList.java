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
 * @author Pavel Dolgov
 * @version $Revision$
 */
package org.apache.harmony.awt;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.EventListener;
import java.util.Iterator;
import java.util.List;

/**
 * List of AWT listeners. It is for 3 purposes.
 * 1. To support list modification from listeners
 * 2. To ensure call for all listeners as atomic operation
 * 3. To support system listeners that are needed for built-in AWT components
 */
public class ListenerList<T> implements Serializable {
    private static final long serialVersionUID = 9180703263299648154L;

    private transient ArrayList<EventListener> systemList;
    private transient ArrayList<T> userList;

    /**
     * Adds system listener to this list.
     *
     * @param listener - listener to be added.
     */
    public void addSystemListener(EventListener listener) {
        if (systemList == null) {
            systemList = new ArrayList<EventListener>();
        }
        systemList.add(listener);
    }

    /**
     * Adds user (public) listener to this list.
     *
     * @param listener - listener to be added.
     */
    public void addUserListener(T listener) {
        if (listener == null) {
            return;
        }
        // transactionally replace old list
        synchronized (this) {
            if (userList == null) {
                userList = new ArrayList<T>();
                userList.add(listener);
                return;
            }
            ArrayList<T> newList = new ArrayList<T>(userList);
            newList.add(listener);
            userList = newList;
        }
    }

    /**
     * Removes user (public) listener to this list.
     *
     * @param listener - listener to be removed.
     */
    public void removeUserListener(Object listener) {
        if (listener == null) {
            return;
        }
        // transactionally replace old list
        synchronized (this) {
            if (userList == null || !userList.contains(listener)) {
                return;
            }
            ArrayList<T> newList = new ArrayList<T>(userList);
            newList.remove(listener);
            userList = (newList.size() > 0 ? newList : null);
        }
    }

    /**
     * Gets all user (public) listeners in one array.
     *
     * @param emptyArray - empty array, it's for deriving particular listeners class.
     * @return array of all user listeners.
     */
    public EventListener[] getUserListeners(EventListener[] emptyArray){
        synchronized (this) {
            return (userList != null ? userList.toArray(emptyArray) : emptyArray);

        }
    }

    /**
     * Gets all user (public) listeners in one list.
     *
     * @return list of all user listeners.
     */
    public List<T> getUserListeners() {
        synchronized (this) {
            return new ArrayList<T>(userList);
        }
    }

    /**
     * Gets iterator for user listeners.
     *
     * @return iterator for user listeners.
     */
    public Iterator<T> getUserIterator() {
        synchronized (this) {
            if (userList == null) {
                List<T> emptyList = Collections.emptyList();
                return emptyList.iterator();
            }
            return new ReadOnlyIterator<T>(userList.iterator());
        }
    }

    /**
     * Gets iterator for system listeners.
     *
     * @return iterator for system listeners.
     */
    public Iterator<EventListener> getSystemIterator() {
        return systemList.iterator();
    }

    private static ArrayList<?> getOnlySerializable(ArrayList<?> list) {
        if (list == null) {
            return null;
        }

        ArrayList<Object> result = new ArrayList<Object>();
        for (Iterator<?> it = list.iterator(); it.hasNext();) {
            Object obj = it.next();
            if (obj instanceof Serializable) {
                result.add(obj);
            }
        }

        return (result.size() != 0) ? result : null;
    }

    private void writeObject(ObjectOutputStream stream) throws IOException {

        stream.defaultWriteObject();

        stream.writeObject(getOnlySerializable(systemList));
        stream.writeObject(getOnlySerializable(userList));
    }

    @SuppressWarnings("unchecked")
    private void readObject(ObjectInputStream stream)
            throws IOException, ClassNotFoundException {

        stream.defaultReadObject();

        systemList = (ArrayList<EventListener>)stream.readObject();
        userList = (ArrayList<T>)stream.readObject();
    }

}
