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

package tests.api.java.io;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;

import junit.framework.TestCase;

/**
 * TODO Type description
 */
public class OpenRandomFileTest extends TestCase {

	public static void main(String[] args) {
		new OpenRandomFileTest().testOpenEmptyFile();
	}

	public OpenRandomFileTest() {
		super();
	}

	public void testOpenNonEmptyFile() {
		try {
			File file = File.createTempFile("test", "tmp");
			assertTrue(file.exists());
			file.deleteOnExit();
			FileOutputStream fos = new FileOutputStream(file);
			fos.write(new byte[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
			fos.close();

			String fileName = file.getCanonicalPath();
			RandomAccessFile raf = new RandomAccessFile(fileName, "rw");
			raf.close();
		} catch (IOException ex) {
			fail(ex.getLocalizedMessage());
		}
	}

	public void testOpenEmptyFile() {
		try {
			File file = File.createTempFile("test", "tmp");
			assertTrue(file.exists());
			file.deleteOnExit();

			String fileName = file.getCanonicalPath();
			RandomAccessFile raf = new RandomAccessFile(fileName, "rw");
			raf.close();
		} catch (IOException ex) {
			fail(ex.getLocalizedMessage());
		}
	}
}
