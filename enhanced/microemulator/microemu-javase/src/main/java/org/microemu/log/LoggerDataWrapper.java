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

package org.microemu.log;

/**
 * @author vlads
 *
 * Convinient method to format debug data
 *
 */
public class LoggerDataWrapper {
	
	private String text; 

	public LoggerDataWrapper(boolean v1) {
		this.text = String.valueOf(v1);	
	}
	
	public LoggerDataWrapper(long v1) {
		this.text = String.valueOf(v1);	
	}

	public LoggerDataWrapper(Object v1) {
		this.text = String.valueOf(v1);
	}

	public LoggerDataWrapper(long v1, long v2) {
		this.text = String.valueOf(v1) + " " + String.valueOf(v2);	
	}
	
	public LoggerDataWrapper(String v1, String v2) {
		this.text = v1 + " " + v2;	
	}
	
	public String toString() {
		return this.text;
	}
}
