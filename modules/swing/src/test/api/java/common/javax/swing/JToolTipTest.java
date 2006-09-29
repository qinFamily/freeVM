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
 * @author Sergey Burlak
 * @version $Revision$
 */
package javax.swing;

import javax.swing.plaf.ToolTipUI;

public class JToolTipTest extends SwingTestCase {
    private JToolTip tooltip;

    protected void setUp() throws Exception {
        tooltip = new JToolTip();
    }

    protected void tearDown() throws Exception {
        tooltip = null;
    }

    public void testGetUIClassID() throws Exception {
        assertEquals("ToolTipUI", tooltip.getUIClassID());
    }

    public void testGetUI() throws Exception {
        assertNotNull(tooltip.getUI());
        assertTrue(tooltip.getUI() instanceof ToolTipUI);
        assertTrue(tooltip.getUI() == tooltip.getUI());
    }

    public void testSetGetTipText() throws Exception {
        assertNull(tooltip.getTipText());
        propertyChangeController = new PropertyChangeController();
        tooltip.addPropertyChangeListener(propertyChangeController);
        tooltip.setTipText("text");
        assertTrue(propertyChangeController.isChanged("tiptext"));
        assertEquals("text", tooltip.getTipText());
        propertyChangeController.reset();
        tooltip.setTipText("text");
        assertFalse(propertyChangeController.isChanged("tiptext"));
        propertyChangeController.reset();
        tooltip.setTipText(null);
        assertTrue(propertyChangeController.isChanged("tiptext"));
        assertNull(tooltip.getTipText());
    }

    public void testSetGetComponent() throws Exception {
        assertEquals(null, tooltip.getComponent());
        propertyChangeController = new PropertyChangeController();
        tooltip.addPropertyChangeListener(propertyChangeController);
        JButton button = new JButton("b");
        tooltip.setComponent(button);
        assertTrue(propertyChangeController.isChanged("component"));
        assertEquals(button, tooltip.getComponent());
        propertyChangeController.reset();
        tooltip.setComponent(button);
        assertFalse(propertyChangeController.isChanged("component"));
        propertyChangeController.reset();
        tooltip.setComponent(null);
        assertTrue(propertyChangeController.isChanged("component"));
        assertNull(tooltip.getComponent());
    }
}
