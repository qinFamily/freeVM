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

import java.awt.Button;

import junit.framework.TestCase;

public class FocusEventTest extends TestCase {

    public final void testFocusEventComponentint() {
        Button button = new Button("Button");
        FocusEvent event = new FocusEvent(button, FocusEvent.FOCUS_GAINED);

        assertEquals(event.getSource(), button);
        assertEquals(event.getID(), FocusEvent.FOCUS_GAINED);
        assertEquals(event.getComponent(), button);
        assertEquals(event.getOppositeComponent(), null);
        assertEquals(event.isTemporary(), false);
    }

    public final void testFocusEventComponentintboolean() {
        Button button = new Button("Button");
        FocusEvent event = new FocusEvent(button, FocusEvent.FOCUS_LOST, true);

        assertEquals(event.getSource(), button);
        assertEquals(event.getID(), FocusEvent.FOCUS_LOST);
        assertEquals(event.getComponent(), button);
        assertEquals(event.getOppositeComponent(), null);
        assertEquals(event.isTemporary(), true);
    }

    public final void testFocusEventComponentintbooleanComponent() {
        Button button = new Button("Button");
        FocusEvent event = new FocusEvent(button, FocusEvent.FOCUS_LOST, true, button);

        assertEquals(event.getSource(), button);
        assertEquals(event.getID(), FocusEvent.FOCUS_LOST);
        assertEquals(event.getComponent(), button);
        assertEquals(event.getOppositeComponent(), button);
        assertEquals(event.isTemporary(), true);
    }

}
