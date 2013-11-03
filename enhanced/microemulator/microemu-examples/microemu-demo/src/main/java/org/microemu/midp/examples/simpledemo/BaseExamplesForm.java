/*
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

package org.microemu.midp.examples.simpledemo;

import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;

public abstract class BaseExamplesForm extends Form implements CommandListener {

	static final Command backCommand = new Command("Back", Command.BACK, 1);

	public BaseExamplesForm(String title) {
		super(title);

		addCommand(backCommand);
		setCommandListener(this);
	}

	public void commandAction(Command c, Displayable d) {
		if (d == this) {
			if (c == backCommand) {
				SimpleDemoMIDlet.showMenu();
			}
		}
	}
}
