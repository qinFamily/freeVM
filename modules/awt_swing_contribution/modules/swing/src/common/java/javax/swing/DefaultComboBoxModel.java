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
 * @author Anton Avtamonov
 * @version $Revision$
 */

package javax.swing;

import java.util.Arrays;
import java.util.Vector;

public class DefaultComboBoxModel extends AbstractListModel implements MutableComboBoxModel {
    private static final long serialVersionUID = 3906372640380892983L;
    private Vector listData;
    private Object selection;

    public DefaultComboBoxModel() {
        listData = new Vector();
    }

    public DefaultComboBoxModel(final Object[] items) {
        listData = new Vector();
        listData.addAll(Arrays.asList(items));
        if (items.length > 0) {
            selection = items[0];
        }
    }

    public DefaultComboBoxModel(final Vector items) {
        listData = items;
        if (items.size() > 0) {
            selection = items.get(0);
        }
    }

    public void addElement(final Object element) {
        listData.add(element);
        fireIntervalAdded(this, listData.size() - 1, listData.size() - 1);
        if (getSelectedItem() == null) {
            setSelectedItem(element);
        }
    }

    public Object getElementAt(final int index) {
        return index < getSize() ? listData.get(index) : null;
    }

    public int getIndexOf(final Object element) {
        return listData.indexOf(element);
    }

    public int getSize() {
        return listData.size();
    }

    public void insertElementAt(final Object element, final int index) {
        listData.insertElementAt(element, index);
        fireIntervalAdded(this, index, index);
    }

    public void removeAllElements() {
        int size = getSize();
        if (size > 0) {
            listData.clear();
            fireIntervalRemoved(this, 0, size - 1);
        }
        selection = null;
    }

    public void removeElement(final Object element) {
        int index = getIndexOf(element);
        if (index != -1) {
            removeElementAt(index);
        }
    }

    public void removeElementAt(final int index) {
        Object removingElement = getElementAt(index);
        listData.remove(index);

        if (selection == removingElement
            || selection != null && selection.equals(removingElement)) {

            if (index == 0 && getSize() > 0) {
                setSelectedItem(getElementAt(0));
            } else if (index > 0) {
                setSelectedItem(getElementAt(index - 1));
            } else {
                setSelectedItem(null);
            }
        }

        fireIntervalRemoved(this, index, index);
    }


    public Object getSelectedItem() {
         return selection;
    }

    public void setSelectedItem(final Object element) {
        if (element != null && !element.equals(selection)
            || element == null && selection != null) {

            selection = element;
            fireContentsChanged(this, -1, -1);
        }
    }
}
