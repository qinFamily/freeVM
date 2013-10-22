/*
    Copyright 2005-2006 The Apache Software Foundation or its licensors, as applicable

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

    See the License for the specific language governing permissions and
    limitations under the License.
*/
/*
 * Created on 18.10.2004
 */

package org.apache.harmony.vmtt.ir.attributes;

/**
 * @author agolubit
 */

public class SourceFile extends Attribute {

	public static final int SOURCEFILE_INDEX = 2; 
	
	private short sourcefile_index;
	
	public SourceFile() {
		super(3);
	}
	
	public short getSourceFileIndex() {
		return sourcefile_index;
	}
	
	public void setSourceFileIndex(short index) {
		sourcefile_index = index;
		setDefined(SOURCEFILE_INDEX);
	}
}