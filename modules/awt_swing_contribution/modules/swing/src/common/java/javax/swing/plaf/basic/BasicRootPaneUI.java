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
 * @author Vadim L. Bogdanov
 * @version $Revision$
 */

package javax.swing.plaf.basic;

import java.awt.event.ActionEvent;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import javax.swing.AbstractAction;
import javax.swing.ActionMap;
import javax.swing.InputMap;
import javax.swing.JComponent;
import javax.swing.JRootPane;
import javax.swing.KeyStroke;
import javax.swing.LookAndFeel;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

import javax.swing.plaf.ActionMapUIResource;
import javax.swing.plaf.ComponentInputMapUIResource;
import javax.swing.plaf.ComponentUI;
import javax.swing.plaf.RootPaneUI;


public class BasicRootPaneUI extends RootPaneUI
        implements PropertyChangeListener {

    /*
     * The place to store the shared instance UI.
     */
    private static BasicRootPaneUI ui;

    /*
     * The class for actions that reside in ActionMap.
     */
    private class DefaultButtonAction extends AbstractAction {
        private JRootPane root;
        private boolean press;

        public DefaultButtonAction(final JRootPane root, final boolean press) {
            this.root = root;
            this.press = press;
        }

        public boolean isEnabled() {
            return root.getDefaultButton().isEnabled();
        }

        public void actionPerformed(final ActionEvent e) {
            if (isEnabled() && press) {
                root.getDefaultButton().doClick(25);
            }
        }
    }

    protected void installListeners(final JRootPane root) {
        root.addPropertyChangeListener(this);
    }

    protected void uninstallListeners(final JRootPane root) {
        root.removePropertyChangeListener(this);
    }

    protected void installKeyboardActions(final JRootPane root) {
        SwingUtilities.replaceUIActionMap(root, getActionMap(root));

        SwingUtilities.replaceUIInputMap(root,
                JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT,
                getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT, root));
        if (root.getDefaultButton() != null) {
            loadDefaultButtonKeyBindings(root);
        }
    }

    protected void uninstallKeyboardActions(final JRootPane root) {
        SwingUtilities.replaceUIActionMap(root, null);
        SwingUtilities.replaceUIInputMap(root,
                JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT, null);
    }

    protected void installDefaults(final JRootPane root) {
        // do nothing
    }

    protected void uninstallDefaults(final JRootPane root) {
        // do nothing
    }

    protected void installComponents(final JRootPane root) {
        // do nothing
    }

    protected void uninstallComponents(final JRootPane root) {
        // do nothing
    }

    public void installUI(final JComponent c) {
        JRootPane root = (JRootPane)c;
        //Install any default property values for color, fonts, borders,
        // icons, opacity, etc. on the component. Whenever possible,
        // property values initialized by the client program should not be overridden
        installDefaults(root);

        //Install a LayoutManager on the component if necessary

        //Create/add any required sub-components to the component
        installComponents(root);

        //Create/install event listeners on the component

        //Create/install a PropertyChangeListener on the component in order to detect and respond to component property changes appropriately
        installListeners(root);

        //Install keyboard UI (mnemonics, traversal, etc.) on the component
        installKeyboardActions(root);

        //Initialize any appropriate instance data
    }

    public void uninstallUI(final JComponent c) {
        JRootPane root = (JRootPane)c;

        //Remove any UI-set borders from the component
        uninstallDefaults(root);

        //Remove any UI-set layout managers on the component

        //Remove any UI-added sub-components from the component
        uninstallComponents(root);

        //Remove any UI-added event/property listeners from the component
        uninstallListeners(root);

        //Remove any UI-installed keyboard UI from the component
        uninstallKeyboardActions(root);

        //Nullify any allocated instance data objects to allow for GC
    }

    public void propertyChange(final PropertyChangeEvent e) {
        if ("defaultButton".equals(e.getPropertyName())) {
            if (e.getNewValue() == null) {
                unloadDefaultButtonKeyBindings((JComponent)e.getSource());
            } else {
                loadDefaultButtonKeyBindings((JComponent)e.getSource());
            }
        }
    }

    public static ComponentUI createUI(final JComponent c) {
        if (ui == null) {
            ui = new BasicRootPaneUI();
        }

        return ui;
    }

    /*
     * Create InputMap and returns it.
     */
    private InputMap getInputMap(final int cond, final JRootPane root) {
        // there is a separage InputMap for every JRootPane (because
        // bindings are removed if defaultButton is set to null),
        // but the bindings are the same
        return createInputMap(cond, root);
    }

    /*
     * Create a new instance of InputMap.
     */
    private InputMap createInputMap(final int cond, final JRootPane root) {
        ComponentInputMapUIResource map = new ComponentInputMapUIResource(root);
        return map;
    }

    /*
     * Create ActionMap if necessary, and return it.
     */
    private ActionMap getActionMap(final JRootPane root) {
        // the action map cannot be shared
        return createActionMap(root);
    }

    /*
     * Create a new instance of ActionMap.
     */
    private ActionMap createActionMap(final JRootPane root) {
        ActionMapUIResource actionMap = new ActionMapUIResource();

        actionMap.put("press", new DefaultButtonAction(root, true));
        actionMap.put("release", new DefaultButtonAction(root, false));

        return actionMap;
    }

    /*
     * Load key bindings from UIDefaults to InputMap. This occurs
     * when the default button is assigned.
     */
    private void loadDefaultButtonKeyBindings(final JComponent root) {
        Object[] bindings = ((Object[])UIManager.get(
                "RootPane.defaultButtonWindowKeyBindings"));

        if (bindings != null) {
            InputMap map = SwingUtilities.getUIInputMap(
                    root, JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);
            LookAndFeel.loadKeyBindings(map, bindings);
        }
    }

    private void unloadDefaultButtonKeyBindings(final JComponent root) {
        Object[] bindings = ((Object[])UIManager.get(
                "RootPane.defaultButtonWindowKeyBindings"));

        if (bindings != null) {
            InputMap map = SwingUtilities.getUIInputMap(
                    root, JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);
            for (int i = 0; i < bindings.length; i += 2) {
                if (bindings[i] instanceof String) {
                    map.remove(KeyStroke.getKeyStroke((String)bindings[i]));
                } else {
                    map.remove((KeyStroke)bindings[i]);
                }
            }
        }
    }
}
