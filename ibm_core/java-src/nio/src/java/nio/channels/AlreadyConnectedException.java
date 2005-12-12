/* Copyright 2004 The Apache Software Foundation or its licensors, as applicable
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package java.nio.channels;


/**
 * Thrown when an attempt is made to connect a SocketChannel that is already connected.
 * 
 */
public class AlreadyConnectedException extends IllegalStateException {

	static final long serialVersionUID = -7331895245053773357L;

	/**
	 * Default constructor.
	 */
	public AlreadyConnectedException() {
		super();
	}
}
