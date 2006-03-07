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

package java.nio;


import org.apache.harmony.luni.platform.Platform;

/**
 * Holds byte order constants.
 * 
 */
public final class ByteOrder {

	/**
	 * This constant represents big endian.
	 */
	public static final ByteOrder BIG_ENDIAN = new ByteOrder("BIG_ENDIAN"); //$NON-NLS-1$

	/**
	 * This constant represents little endian.
	 */
	public static final ByteOrder LITTLE_ENDIAN = new ByteOrder("LITTLE_ENDIAN"); //$NON-NLS-1$

	private static final ByteOrder NATIVE_ORDER;

	static {
		if (Platform.getMemorySystem().isLittleEndian()) {
			NATIVE_ORDER = LITTLE_ENDIAN;
		} else {
			NATIVE_ORDER = BIG_ENDIAN;
		}
	}

	/**
	 * Answers the current platform byte order.
	 * 
	 * @return the byte order object, which is either identical to LITTLE_ENDIAN
	 *         or BIG_ENDIAN.
	 */
	public static ByteOrder nativeOrder() {
		return NATIVE_ORDER;
	}

	private final String name;

	private ByteOrder(String name) {
		super();
		this.name = name;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	public String toString() {
		return name;
	}
}
