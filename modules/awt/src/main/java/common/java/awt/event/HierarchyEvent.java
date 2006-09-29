/*
 *  Copyright 2005 - 2006 The Apache Software Foundation or its licensors, as applicable.
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
 * @author Michael Danilov
 * @version $Revision$
 */
package java.awt.event;

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.Container;

public class HierarchyEvent extends AWTEvent {

    private static final long serialVersionUID = -5337576970038043990L;

    public static final int HIERARCHY_FIRST = 1400;

    public static final int HIERARCHY_CHANGED = 1400;

    public static final int ANCESTOR_MOVED = 1401;

    public static final int ANCESTOR_RESIZED = 1402;

    public static final int HIERARCHY_LAST = 1402;

    public static final int PARENT_CHANGED = 1;

    public static final int DISPLAYABILITY_CHANGED = 2;

    public static final int SHOWING_CHANGED = 4;

    private Container changedParent;
    private Component changed;
    private long changeFlag;

    public HierarchyEvent(Component source, int id, Component changed, 
                          Container changedParent) {
        this(source, id, changed, changedParent, 0l);
    }

    public HierarchyEvent(Component source, int id, Component changed,
                          Container changedParent, long changeFlags) {
        super(source, id);

        this.changed = changed;
        this.changedParent = changedParent;
        this.changeFlag = changeFlags;
    }

    public Component getComponent() {
        return (Component) source;
    }

    public long getChangeFlags() {
        return changeFlag;
    }

    public Component getChanged() {
        return changed;
    }

    public Container getChangedParent() {
        return changedParent;
    }

    public String paramString() {
        /* The format is based on 1.5 release behavior 
         * which can be revealed by the following code:
         * 
         * HierarchyEvent e = new HierarchyEvent(new Button("Button"),
         *          HierarchyEvent.HIERARCHY_CHANGED,
         *          new Panel(), new Container());
         * System.out.println(e);
         */
        String paramString = null;

        switch (id) {
        case HIERARCHY_CHANGED:
            paramString = "HIERARCHY_CHANGED";
            break;
        case ANCESTOR_MOVED:
            paramString = "ANCESTOR_MOVED";
            break;
        case ANCESTOR_RESIZED:
            paramString = "ANCESTOR_RESIZED";
            break;
        default:
            paramString = "unknown type";
        }

        paramString += " (";

        if (id == HIERARCHY_CHANGED) {
            if ((changeFlag & PARENT_CHANGED) > 0) {
                paramString += "PARENT_CHANGED,";
            }
            if ((changeFlag & DISPLAYABILITY_CHANGED) > 0) {
                paramString += "DISPLAYABILITY_CHANGED,";
            }
            if ((changeFlag & SHOWING_CHANGED) > 0) {
                paramString += "SHOWING_CHANGED,";
            }
        }

        return paramString + "changed=" + changed + 
                ",changedParent=" + changedParent + ")";
    }

}
