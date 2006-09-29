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
* @author Alexander T. Simbirtsev
* @version $Revision$
*/
package javax.swing.plaf.basic;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.JComponent;
import javax.swing.plaf.ComponentUI;

public class BasicPopupMenuSeparatorUI extends BasicSeparatorUI {

    public void paint(final Graphics g, final JComponent c) {
        Color oldColor = g.getColor();
        Dimension size = c.getSize();
        g.setColor(c.getForeground());
        g.drawLine(0, 0, size.width, 0);
        g.setColor(c.getBackground());
        g.drawLine(0, 1, size.width, 1);
        g.setColor(oldColor);
    }

    public static ComponentUI createUI(final JComponent c) {
        return new BasicPopupMenuSeparatorUI();
    }

    public Dimension getPreferredSize(final JComponent c) {
        return new Dimension(0, 2);
    }

}
