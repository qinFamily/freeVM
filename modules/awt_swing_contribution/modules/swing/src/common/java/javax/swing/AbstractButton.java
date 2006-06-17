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
 * @author Alexander T. Simbirtsev
 * @version $Revision$
 */
package javax.swing;

import java.awt.Graphics;
import java.awt.Image;
import java.awt.Insets;
import java.awt.ItemSelectable;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.Serializable;

import javax.accessibility.AccessibleAction;
import javax.accessibility.AccessibleExtendedComponent;
import javax.accessibility.AccessibleIcon;
import javax.accessibility.AccessibleKeyBinding;
import javax.accessibility.AccessibleRelationSet;
import javax.accessibility.AccessibleState;
import javax.accessibility.AccessibleStateSet;
import javax.accessibility.AccessibleText;
import javax.accessibility.AccessibleValue;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.plaf.ButtonUI;
import javax.swing.text.AttributeSet;

import org.apache.harmony.x.swing.ButtonCommons;
import org.apache.harmony.x.swing.StringConstants;
import org.apache.harmony.x.swing.Utilities;


public abstract class AbstractButton extends JComponent implements ItemSelectable, SwingConstants {

    protected abstract class AccessibleAbstractButton extends AccessibleJComponent
                    implements AccessibleAction, AccessibleValue, AccessibleText,
                    AccessibleExtendedComponent, Serializable {

        public AccessibleKeyBinding getAccessibleKeyBinding() {
            return null;
        }

        public int getAccessibleActionCount() {
            return 1;
        }

        public String getToolTipText() {
            return AbstractButton.this.getToolTipText();
        }
        public AccessibleValue getAccessibleValue() {
            return this;
        }

        public AccessibleText getAccessibleText() {
            return null;
        }

        public String getAccessibleName() {
            return (super.getAccessibleName() != null) ? super.getAccessibleName() : getText();
        }

        public AccessibleRelationSet getAccessibleRelationSet() {
            return super.getAccessibleRelationSet();
        }

        public String getTitledBorderText() {
            return super.getTitledBorderText();
        }

        public AccessibleStateSet getAccessibleStateSet() {
            AccessibleStateSet set = super.getAccessibleStateSet();
            if (isSelected()) {
                set.add(AccessibleState.CHECKED);
            }

            return set;
        }

        public AccessibleIcon[] getAccessibleIcon() {
            if (icon != null && icon instanceof ImageIcon) {
                return new AccessibleIcon[] { (AccessibleIcon)((ImageIcon)icon).getAccessibleContext() };
            }

            return null;
        }

        public AccessibleAction getAccessibleAction() {
            return this;
        }

        public boolean doAccessibleAction(final int index) {
            if (0 <= index && index < getAccessibleActionCount()) {
                return true;
            }

            return false;
        }

        public String getAccessibleActionDescription(final int index) {
            if (0 <= index && index < getAccessibleActionCount()) {
                return "click";
            }

            return null;
        }

        public Number getCurrentAccessibleValue() {
            return (AbstractButton.this.isSelected()) ? new Integer(1) : new Integer(0);
        }

        public Number getMaximumAccessibleValue() {
            return new Integer(1);
        }

        public Number getMinimumAccessibleValue() {
            return new Integer(0);
        }

        public boolean setCurrentAccessibleValue(final Number value) {
            boolean valueSet = (value.intValue() == 0) ? false : true;
            if (valueSet != isSelected()) {
                setSelected(valueSet);
                if (valueSet) {
                    firePropertyChange("AccessibleState", null, AccessibleState.SELECTED);
                    firePropertyChange("AccessibleValue", new Integer(0), new Integer(1));
                } else {
                    firePropertyChange("AccessibleState", AccessibleState.SELECTED, null);
                    firePropertyChange("AccessibleValue", new Integer(1), new Integer(0));
                }
            }

            return true;
        }

        public int getCaretPosition() {
            return -1;
        }

        public int getCharCount() {
            String text = AbstractButton.this.getText();
            return (text != null) ? text.length() : 0;
        }

        public int getSelectionEnd() {
            return -1;
        }

        public int getSelectionStart() {
            return -1;
        }

        public int getIndexAtPoint(final Point point) {
            return -1;
        }

        public Rectangle getCharacterBounds(final int arg0) {
            return null;
        }

        public String getSelectedText() {
            return null;
        }

        public String getAfterIndex(final int part, final int index) {
            return null;
        }

        public String getAtIndex(final int part, final int index) {
            return null;
        }

        public String getBeforeIndex(final int part, final int index) {
            return null;
        }

        public AttributeSet getCharacterAttribute(final int index) {
            return null;
        }
    };

    protected class ButtonChangeListener implements ChangeListener, Serializable {
        private ButtonChangeListener() {
        }

        public void stateChanged(final ChangeEvent event) {
            int mn = model.getMnemonic();
            updateMnemonic(mn, Utilities.keyCodeToKeyChar(mn));
            fireStateChanged();
        }
    };

    final class ActionAndModelListener implements ItemListener, ActionListener, PropertyChangeListener, Serializable {
        public void itemStateChanged(final ItemEvent event) {
            fireItemStateChanged(event);
        }

        public void actionPerformed(final ActionEvent event) {
            fireActionPerformed(event);
        }

        public void propertyChange(final PropertyChangeEvent event) {
            configurePropertyFromAction((Action)event.getSource(), event.getPropertyName());
        }
    };

    public static final String MODEL_CHANGED_PROPERTY = "model";
    public static final String TEXT_CHANGED_PROPERTY = "text";
    public static final String MNEMONIC_CHANGED_PROPERTY = "mnemonic";
    public static final String MARGIN_CHANGED_PROPERTY = "margin";
    public static final String VERTICAL_ALIGNMENT_CHANGED_PROPERTY = "verticalAlignment";
    public static final String HORIZONTAL_ALIGNMENT_CHANGED_PROPERTY = "horizontalAlignment";
    public static final String VERTICAL_TEXT_POSITION_CHANGED_PROPERTY = "verticalTextPosition";
    public static final String HORIZONTAL_TEXT_POSITION_CHANGED_PROPERTY = "horizontalTextPosition";
    public static final String BORDER_PAINTED_CHANGED_PROPERTY = "borderPainted";
    public static final String FOCUS_PAINTED_CHANGED_PROPERTY = "focusPainted";
    public static final String ROLLOVER_ENABLED_CHANGED_PROPERTY = "rolloverEnabled";
    public static final String CONTENT_AREA_FILLED_CHANGED_PROPERTY = "contentAreaFilled";
    public static final String ICON_CHANGED_PROPERTY = "icon";
    public static final String PRESSED_ICON_CHANGED_PROPERTY = "pressedIcon";
    public static final String SELECTED_ICON_CHANGED_PROPERTY = "selectedIcon";
    public static final String ROLLOVER_ICON_CHANGED_PROPERTY = "rolloverIcon";
    public static final String ROLLOVER_SELECTED_ICON_CHANGED_PROPERTY = "rolloverSelectedIcon";
    public static final String DISABLED_ICON_CHANGED_PROPERTY = "disabledIcon";
    public static final String DISABLED_SELECTED_ICON_CHANGED_PROPERTY = "disabledSelectedIcon";

    private static final Object ALL_ACTION_PROPERTIES = new Object() {
        public boolean equals(final Object o) {
            return true;
        }
    };

    private static final Action CLEAR_ACTION_PROPERTIES = new AbstractAction() {
        public void actionPerformed(final ActionEvent e) {}
        public void putValue(final String name, final Object value) {}
        public void setEnabled(final boolean enabled) {}
    };

    protected transient ChangeEvent changeEvent = new ChangeEvent(this);

    protected ButtonModel model;

    protected ChangeListener changeListener = createChangeListener();
    protected ActionListener actionListener = createActionListener();
    protected ItemListener itemListener = createItemListener();

    private PropertyChangeListener actionPropertyChangeListener;
    private ActionAndModelListener handler;

    private String text = "";
    private Insets margin;
    private Action action;

    private Icon icon;
    private Icon pressedIcon;
    private Icon disabledIcon;
    private Icon defaultDisabledIcon;
    private Icon selectedIcon;
    private Icon disabledSelectedIcon;
    private Icon defaultDisabledSelectedIcon;
    private Icon rolloverIcon;
    private Icon rolloverSelectedIcon;

    private boolean borderPainted = true;
    private boolean focusPainted = true;
    private boolean rolloverEnabled;
    private boolean contentAreaFilled = true;

    private int verticalAlignment = SwingConstants.CENTER;
    private int horizontalAlignment = SwingConstants.CENTER;
    private int verticalTextPosition = SwingConstants.CENTER;
    private int horizontalTextPosition = SwingConstants.TRAILING;

    private int iconTextGap = 4;
    private int mnemonic;
    private int mnemonicIndex = -1;
    private long multiClickThreshhold;

    protected void init(final String text, final Icon icon) {
        if (text != null) {
            setText(text);
        }
        if (icon != null) {
            setIcon(icon);
        }
        updateUI();
    }

    protected PropertyChangeListener createActionPropertyChangeListener(final Action action) {
        return (handler != null) ? handler : (handler = new ActionAndModelListener());
    }

    public void setUI(final ButtonUI ui) {
        super.setUI(ui);
    }

    public ButtonUI getUI() {
        return (ButtonUI)ui;
    }

    public void removeChangeListener(final ChangeListener listener) {
        listenerList.remove(ChangeListener.class, listener);
    }

    public void addChangeListener(final ChangeListener listener) {
        listenerList.add(ChangeListener.class, listener);
    }

    public ChangeListener[] getChangeListeners() {
        return (ChangeListener[])listenerList.getListeners(ChangeListener.class);
    }

    protected ChangeListener createChangeListener() {
        return new ButtonChangeListener();
    }

    public void setSelectedIcon(final Icon selectedIcon) {
        Icon oldValue = this.selectedIcon;
        this.selectedIcon = selectedIcon;
        resetDefaultDisabledIcons();
        firePropertyChange(SELECTED_ICON_CHANGED_PROPERTY, oldValue, selectedIcon);
    }

    public void setRolloverSelectedIcon(final Icon rolloverSelectedIcon) {
        if (this.rolloverSelectedIcon != rolloverSelectedIcon) {
            Icon oldValue = this.rolloverSelectedIcon;
            this.rolloverSelectedIcon = rolloverSelectedIcon;
            firePropertyChange(ROLLOVER_SELECTED_ICON_CHANGED_PROPERTY, oldValue, rolloverSelectedIcon);
            setRolloverEnabled(true);
        }
    }

    public void setRolloverIcon(final Icon rolloverIcon) {
        if (this.rolloverIcon != rolloverIcon) {
            Icon oldValue = this.rolloverIcon;
            this.rolloverIcon = rolloverIcon;
            firePropertyChange(ROLLOVER_ICON_CHANGED_PROPERTY, oldValue, rolloverIcon);
            setRolloverEnabled(true);
        }
    }

    public void setPressedIcon(final Icon pressedIcon) {
        Icon oldValue = this.pressedIcon;
        this.pressedIcon = pressedIcon;
        firePropertyChange(PRESSED_ICON_CHANGED_PROPERTY, oldValue, pressedIcon);
    }

    private void resetDefaultDisabledIcons() {
        defaultDisabledIcon = null;
        defaultDisabledSelectedIcon = null;
    }

    public void setIcon(final Icon icon) {
        Icon oldValue = this.icon;
        this.icon = icon;
        resetDefaultDisabledIcons();
        firePropertyChange(ICON_CHANGED_PROPERTY, oldValue, icon);
    }

    public void setDisabledSelectedIcon(final Icon disabledSelectedIcon) {
        Icon oldValue = this.disabledSelectedIcon;
        this.disabledSelectedIcon = disabledSelectedIcon;
        resetDefaultDisabledIcons();
        firePropertyChange(DISABLED_SELECTED_ICON_CHANGED_PROPERTY, oldValue, disabledSelectedIcon);
    }

    public void setDisabledIcon(final Icon disabledIcon) {
        Icon oldValue = this.disabledIcon;
        this.disabledIcon = disabledIcon;
        resetDefaultDisabledIcons();
        firePropertyChange(DISABLED_ICON_CHANGED_PROPERTY, oldValue, disabledIcon);
    }

    public Icon getSelectedIcon() {
        return selectedIcon;
    }

    public Icon getRolloverSelectedIcon() {
        return rolloverSelectedIcon;
    }

    public Icon getRolloverIcon() {
        return rolloverIcon;
    }

    public Icon getPressedIcon() {
        return pressedIcon;
    }

    public Icon getIcon() {
        return icon;
    }

    private Icon createDefaultDisabledSelectedIcon() {
        if (defaultDisabledSelectedIcon != null) {
            return defaultDisabledSelectedIcon;
        }

        if (selectedIcon instanceof ImageIcon) {
            defaultDisabledIcon = new ImageIcon(GrayFilter.createDisabledImage(((ImageIcon)selectedIcon).getImage()));
        } else if (disabledIcon instanceof ImageIcon) {
            defaultDisabledIcon = new ImageIcon(GrayFilter.createDisabledImage(((ImageIcon)disabledIcon).getImage()));
        } else if (icon instanceof ImageIcon) {
            defaultDisabledIcon = new ImageIcon(GrayFilter.createDisabledImage(((ImageIcon)icon).getImage()));
        }
        return defaultDisabledIcon;
    }

    public Icon getDisabledSelectedIcon() {
        return (disabledSelectedIcon != null) ? disabledSelectedIcon : createDefaultDisabledSelectedIcon();
    }

    private Icon createDefaultDisabledIcon() {
        if (defaultDisabledIcon != null) {
            return defaultDisabledIcon;
        }
        if (icon instanceof ImageIcon) {
            defaultDisabledIcon = new ImageIcon(GrayFilter.createDisabledImage(((ImageIcon)icon).getImage()));
        }

        return defaultDisabledIcon;
    }

    public Icon getDisabledIcon() {
        return (disabledIcon != null) ? disabledIcon : createDefaultDisabledIcon();
    }

    public void setModel(final ButtonModel m) {
        if (model != m) {
            ButtonModel oldValue = model;

            if (model != null) {
                model.removeActionListener(actionListener);
                model.removeItemListener(itemListener);
                model.removeChangeListener(changeListener);
            }
            model = m;
            if (model != null) {
                model.addChangeListener(changeListener);
                model.addItemListener(itemListener);
                model.addActionListener(actionListener);
                int mn = model.getMnemonic();
                updateMnemonic(mn, Utilities.keyCodeToKeyChar(mn));
            }
            firePropertyChange(MODEL_CHANGED_PROPERTY, oldValue, model);
        }
    }

    public ButtonModel getModel() {
        return model;
    }

    public void setAction(final Action action) {
        if (this.action == action && action != null) {
            return;
        }
        Action oldValue = this.action;
        if (oldValue != null) {
            if (hasListener(Action.class, oldValue)) {
                removeActionListener(oldValue);
            }
            if (actionPropertyChangeListener != null) {
                oldValue.removePropertyChangeListener(actionPropertyChangeListener);
            }
        }

        this.action = action;
        if (action != null) {
            if (!hasListener(ActionListener.class, action)) {
                listenerList.add(Action.class, action);
                addActionListener(action);
            }
            actionPropertyChangeListener = createActionPropertyChangeListener(action);
            action.addPropertyChangeListener(actionPropertyChangeListener);
        }

        firePropertyChange(StringConstants.ACTION_PROPERTY_CHANGED, oldValue, action);
        configurePropertiesFromAction(action);
    }

    void configurePropertyFromAction(final Action action, final Object propertyName) {
        if (propertyName == null) {
            return;
        }
        if (propertyName.equals(Action.MNEMONIC_KEY)) {
            Object actionMnemonic = action.getValue(Action.MNEMONIC_KEY);
            setMnemonic((actionMnemonic != null) ? ((Integer)actionMnemonic).intValue() : 0);
        }
        if (propertyName.equals(Action.SHORT_DESCRIPTION)) {
            setToolTipText((String)action.getValue(Action.SHORT_DESCRIPTION));
        }
        if (propertyName.equals(Action.SMALL_ICON)) {
            setIcon((Icon)action.getValue(Action.SMALL_ICON));
        }
        if (propertyName.equals(StringConstants.ENABLED_PROPERTY_CHANGED)) {
            setEnabled(action.isEnabled());
        }
        if (propertyName.equals(Action.NAME)) {
            setText((String)action.getValue(Action.NAME));
        }
        if (propertyName.equals(Action.ACTION_COMMAND_KEY)) {
            setActionCommand((String)action.getValue(Action.ACTION_COMMAND_KEY));
        }
    }

    protected void configurePropertiesFromAction(final Action action) {
        final Action a = (action != null) ? action : CLEAR_ACTION_PROPERTIES;
        configurePropertyFromAction(a, getActionPropertiesFilter());
    }

    public Action getAction() {
        return action;
    }

    public void setText(final String text) {
        if (text != this.text) {
            String oldValue = this.text;
            this.text = text;
            firePropertyChange(TEXT_CHANGED_PROPERTY, oldValue, text);
            updateDisplayedMnemonicsIndex(Utilities.keyCodeToKeyChar(mnemonic));
        }
    }

    /**
     * @deprecated
     */
    public void setLabel(final String label) {
        setText(label);
    }

    public void setActionCommand(final String command) {
        model.setActionCommand(command);
    }

    protected int checkVerticalKey(final int key, final String exceptionText) {
        return Utilities.checkVerticalKey(key, exceptionText);
    }

    protected int checkHorizontalKey(final int key, final String exceptionText) {
        return Utilities.checkHorizontalKey(key, exceptionText);
    }

    public String getText() {
        return text;
    }

    /**
     * @deprecated
     */
    public String getLabel() {
        return getText();
    }

    public String getActionCommand() {
        String command = model.getActionCommand();
        return (command != null) ? command : getText();
    }

    public Object[] getSelectedObjects() {
        return model.isSelected() ? new Object[]{ getText() } : null;
    }

    public void removeItemListener(final ItemListener listener) {
        listenerList.remove(ItemListener.class, listener);
    }

    public void addItemListener(final ItemListener listener) {
        listenerList.add(ItemListener.class, listener);
    }

    public ItemListener[] getItemListeners() {
        return (ItemListener[])listenerList.getListeners(ItemListener.class);
    }

    protected ItemListener createItemListener() {
        return (handler != null) ? handler : (handler = new ActionAndModelListener());
    }

    protected void fireItemStateChanged(final ItemEvent event) {
        ItemListener[] listeners = getItemListeners();
        if (listeners.length > 0) {
            ItemEvent itemEvent = new ItemEvent(this, ItemEvent.ITEM_STATE_CHANGED, this, event.getStateChange());
            for (int i = 0; i < listeners.length; i++ ){
                listeners[i].itemStateChanged(itemEvent);
            }
        }
    }

    public void removeActionListener(final ActionListener listener) {
        listenerList.remove(ActionListener.class, listener);
    }

    public void addActionListener(final ActionListener listener) {
        listenerList.add(ActionListener.class, listener);
    }

    public ActionListener[] getActionListeners() {
        return (ActionListener[])listenerList.getListeners(ActionListener.class);
    }

    protected ActionListener createActionListener() {
        return (handler != null) ? handler : (handler = new ActionAndModelListener());
    }

    protected void fireActionPerformed(final ActionEvent event) {
        ActionListener[] listeners = getActionListeners();
        if (listeners.length > 0) {
            String command = (event.getActionCommand() != null) ? event.getActionCommand() : getText();
            ActionEvent actionEvent = new ActionEvent(this, ActionEvent.ACTION_PERFORMED, command, event.getModifiers());
            for (int i = 0; i < listeners.length; i++ ){
                listeners[i].actionPerformed(actionEvent);
            }
        }
    }

    public void setMargin(final Insets margin) {
        Insets oldValue = this.margin;
        this.margin = margin;
        firePropertyChange(MARGIN_CHANGED_PROPERTY, oldValue, margin);
    }

    public Insets getMargin() {
        return margin;
    }

    public boolean imageUpdate(final Image img, final int infoflags, final int x, final int y, final int w, final int h) {
        Icon curIcon = ButtonCommons.getCurrentIcon(this);
        if ((curIcon == null) ||
            !(curIcon instanceof ImageIcon) || (((ImageIcon)curIcon).getImage() != img)) {

            return false;
        }

        return super.imageUpdate(img, infoflags, x, y, w, h);
    }

    protected void paintBorder(final Graphics g) {
        if (isBorderPainted()) {
            super.paintBorder(g);
        }
    }

    public void doClick(final int pressTime) {
        final ButtonModel model = getModel();

        model.setArmed(true);
        model.setPressed(true);
        if (pressTime > 0) {
            paintImmediately(0, 0, getWidth(), getHeight());
            try {
                Thread.sleep(pressTime);
            } catch (InterruptedException e) {}
        }
        model.setPressed(false);
        model.setArmed(false);
    }

    /*
     * The click delay is based on 1.5 release behavior
     * which can be revealed using the following code:
     *
     *  AbstractButton ab = new AbstractButton() {};
     *  long startTime = System.currentTimeMillis();
     *  ab.setModel(new DefaultButtonModel());
     *  for (int i = 0; i < 100; i++) {
     *      ab.doClick();
     *  }
     *  long stopTime = System.currentTimeMillis();
     *  System.err.println("doClick takes " + (stopTime - startTime) / 100);
     */
    public void doClick() {
        doClick(70);
    }

    public void setSelected(final boolean selected) {
        model.setSelected(selected);
    }

    public void setRolloverEnabled(final boolean rollover) {
        boolean oldValue = rolloverEnabled;
        rolloverEnabled = rollover;
        firePropertyChange(ROLLOVER_ENABLED_CHANGED_PROPERTY, oldValue, rolloverEnabled);
    }

    public void setFocusPainted(final boolean painted) {
        boolean oldValue = focusPainted;
        focusPainted = painted;
        firePropertyChange(FOCUS_PAINTED_CHANGED_PROPERTY, oldValue, painted);
    }

    public void setEnabled(final boolean enabled) {
        if (model.isEnabled() != enabled) {
            model.setEnabled(enabled);
            super.setEnabled(enabled);
        }
    }

    public void setContentAreaFilled(final boolean filled) {
        boolean oldValue = contentAreaFilled;
        contentAreaFilled = filled;
        firePropertyChange(CONTENT_AREA_FILLED_CHANGED_PROPERTY, oldValue, contentAreaFilled);
    }

    public void setBorderPainted(final boolean painted) {
        boolean oldValue = borderPainted;
        borderPainted = painted;
        firePropertyChange(BORDER_PAINTED_CHANGED_PROPERTY, oldValue, borderPainted);
    }

    public void setMultiClickThreshhold(final long threshold) {
        if (threshold < 0) {
            throw new IllegalArgumentException("threshhold must be >= 0");
        }
        multiClickThreshhold = threshold;
    }

    public void setVerticalTextPosition(final int pos) {
        int oldValue = verticalTextPosition;
        verticalTextPosition = checkVerticalKey(pos, VERTICAL_TEXT_POSITION_CHANGED_PROPERTY);
        firePropertyChange(VERTICAL_TEXT_POSITION_CHANGED_PROPERTY, oldValue, verticalTextPosition);
    }

    public void setVerticalAlignment(final int alignment) {
        int oldValue = verticalAlignment;
        verticalAlignment = checkVerticalKey(alignment, VERTICAL_ALIGNMENT_CHANGED_PROPERTY);
        firePropertyChange(VERTICAL_ALIGNMENT_CHANGED_PROPERTY, oldValue, verticalAlignment);
    }

    public void setMnemonic(final char keyChar) {
        setMnemonic(Utilities.keyCharToKeyCode(keyChar), keyChar);
    }

    public void setMnemonic(final int mnemonicCode) {
        setMnemonic(mnemonicCode, Utilities.keyCodeToKeyChar(mnemonicCode));
    }

    private void setMnemonic(final int keyCode, final char keyChar) {
        model.setMnemonic(keyCode);
    }

    private void updateMnemonic(final int keyCode, final char keyChar) {
        int oldKeyCode = mnemonic;
        if (oldKeyCode == keyCode) {
            return;
        }
        mnemonic = keyCode;
        firePropertyChange(MNEMONIC_CHANGED_PROPERTY, oldKeyCode, keyCode);
        updateDisplayedMnemonicsIndex(keyChar);
    }

    private void updateDisplayedMnemonicsIndex(final char keyChar) {
        setDisplayedMnemonicIndex(Utilities.getDisplayedMnemonicIndex(text, keyChar));
    }

    public int getMnemonic() {
        return mnemonic;
    }

    public void setDisplayedMnemonicIndex(final int index) throws IllegalArgumentException {
        if (index < -1 || index >=0 && (text == null || index >= text.length())) {
            throw new IllegalArgumentException("index == " + index);
        }

        int oldValue = mnemonicIndex;
        mnemonicIndex = index;
        firePropertyChange(StringConstants.MNEMONIC_INDEX_PROPERTY_CHANGED,
                           oldValue, index);
    }

    public int getDisplayedMnemonicIndex() {
        return mnemonicIndex;
    }

    public void setIconTextGap(final int gap) {
        LookAndFeel.markPropertyNotInstallable(this, StringConstants.ICON_TEXT_GAP_PROPERTY_CHANGED);
        int oldValue = iconTextGap;
        iconTextGap = gap;
        firePropertyChange(StringConstants.ICON_TEXT_GAP_PROPERTY_CHANGED, oldValue, iconTextGap);
    }

    public void setHorizontalTextPosition(final int pos) {
        int oldValue = horizontalTextPosition;
        horizontalTextPosition = checkHorizontalKey(pos, HORIZONTAL_TEXT_POSITION_CHANGED_PROPERTY);
        firePropertyChange(HORIZONTAL_TEXT_POSITION_CHANGED_PROPERTY, oldValue, horizontalTextPosition);
    }

    public void setHorizontalAlignment(final int alignment) {
        int oldValue = horizontalAlignment;
        horizontalAlignment = checkHorizontalKey(alignment, HORIZONTAL_ALIGNMENT_CHANGED_PROPERTY);
        firePropertyChange(HORIZONTAL_ALIGNMENT_CHANGED_PROPERTY, oldValue, horizontalAlignment);
    }

    public boolean isSelected() {
        return model.isSelected();
    }

    public boolean isRolloverEnabled() {
        return rolloverEnabled;
    }

    public boolean isFocusPainted() {
        return focusPainted;
    }

    public boolean isContentAreaFilled() {
        return contentAreaFilled;
    }

    public boolean isBorderPainted() {
        return borderPainted;
    }

    protected void fireStateChanged() {
        ChangeListener[] listeners = getChangeListeners();
        for (int i = 0; i < listeners.length; i++ ){
            listeners[i].stateChanged(changeEvent);
        }
    }

    public long getMultiClickThreshhold() {
        return multiClickThreshhold;
    }

    public int getVerticalTextPosition() {
        return verticalTextPosition;
    }

    public int getVerticalAlignment() {
        return verticalAlignment;
    }

    public int getIconTextGap() {
        return iconTextGap;
    }

    public int getHorizontalTextPosition() {
        return horizontalTextPosition;
    }

    public int getHorizontalAlignment() {
        return horizontalAlignment;
    }

    Object getActionPropertiesFilter() {
        return ALL_ACTION_PROPERTIES;
    }

    boolean processMnemonics(final KeyEvent event) {
        final KeyStroke keyStroke = KeyStroke.getKeyStrokeForEvent(event);
        if (keyStroke.isOnKeyRelease() || getMnemonic() == 0) {
            return false;
        }

        if (isMnemonicKeyStroke(keyStroke)) {
            Action action = getActionMap().get(StringConstants.MNEMONIC_ACTION);
            if (action != null) {
                SwingUtilities.notifyAction(action, keyStroke, event, this, event.getModifiersEx());
                return true;
            }
        }

        return false;
    }

    boolean isMnemonicKeyStroke(final KeyStroke keyStroke) {
        return keyStroke.getKeyCode() == getMnemonic() && (keyStroke.getModifiers() & InputEvent.ALT_DOWN_MASK) != 0;
    }
}

