/* Copyright 2002, 2005 The Apache Software Foundation or its licensors, as applicable
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

package tests.support;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

/**
 * This class implements the Support_Socket interface using java.net Sockets.
 */
public class Support_HttpSocket implements Support_Socket {

	private Socket instance;

	private boolean streamOpen = false;

	Support_HttpSocket(Socket socket) {
		instance = socket;
	}

	public InputStream getInputStream() throws IOException {
		streamOpen = true;
		return instance.getInputStream();
	}

	public OutputStream getOutputStream() throws IOException {
		streamOpen = true;
		return instance.getOutputStream();
	}

	public void close() throws IOException {
		if (!streamOpen && instance != null)
			instance.close();
	}

}
