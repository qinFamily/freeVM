/* Copyright 1998, 2005 The Apache Software Foundation or its licensors, as applicable
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
package org.apache.harmony.archive.tests.java.util.zip;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.zip.Checksum;
import java.util.zip.GZIPOutputStream;

public class GZIPOutputStreamTest extends junit.framework.TestCase {

	class TestGZIPOutputStream extends GZIPOutputStream {
		TestGZIPOutputStream(OutputStream out) throws IOException {
			super(out);
		}

		TestGZIPOutputStream(OutputStream out, int size) throws IOException {
			super(out, size);
		}

		Checksum getChecksum() {
			return crc;
		}
	}

	/**
	 * @tests java.util.zip.GZIPOutputStream#GZIPOutputStream(java.io.OutputStream)
	 */
	public void test_ConstructorLjava_io_OutputStream() {
		try {
			FileOutputStream outFile = new FileOutputStream("GZIPOutCon.txt");
			TestGZIPOutputStream outGZIP = new TestGZIPOutputStream(outFile);
			assertNotNull("the constructor for GZIPOutputStream is null",
					outGZIP);
			assertEquals("the CRC value of the outputStream is not zero", 0, outGZIP
					.getChecksum().getValue());
			outGZIP.close();
		} catch (IOException e) {
			fail(
					"an IO error occured while trying to find the output file or creating GZIP constructor");
		}
	}

	/**
	 * @tests java.util.zip.GZIPOutputStream#GZIPOutputStream(java.io.OutputStream,
	 *        int)
	 */
	public void test_ConstructorLjava_io_OutputStreamI() {
		try {
			FileOutputStream outFile = new FileOutputStream("GZIPOutCon.txt");
			TestGZIPOutputStream outGZIP = new TestGZIPOutputStream(outFile,
					100);
			assertNotNull("the constructor for GZIPOutputStream is null",
					outGZIP);
			assertEquals("the CRC value of the outputStream is not zero", 0, outGZIP
					.getChecksum().getValue());
			outGZIP.close();
		} catch (IOException e) {
			fail(
					"an IO error occured while trying to find the output file or creating GZIP constructor");
		}
	}

	/**
	 * @tests java.util.zip.GZIPOutputStream#finish()
	 */
	public void test_finish() {
		// test method java.util.zip.GZIPOutputStream.finish()
		byte byteArray[] = { 3, 5, 2, 'r', 'g', 'e', 'f', 'd', 'e', 'w' };
		try {
			FileOutputStream outFile = new FileOutputStream("GZIPOutFinish.txt");
			TestGZIPOutputStream outGZIP = new TestGZIPOutputStream(outFile);

			outGZIP.finish();
			int r = 0;
			try {
				outGZIP.write(byteArray, 0, 1);
			} catch (IOException e) {
				r = 1;
			}

			assertEquals("GZIP instance can still be used after finish is called",
					1, r);
			outGZIP.close();
		} catch (IOException e) {
			fail(
					"an IO error occured while trying to find the output file or creating GZIP constructor");
		}
	}

	/**
	 * @tests java.util.zip.GZIPOutputStream#close()
	 */
	public void test_close() {
		// test method java.util.zip.GZIPOutputStream.close()
		byte byteArray[] = { 3, 5, 2, 'r', 'g', 'e', 'f', 'd', 'e', 'w' };
		try {
			FileOutputStream outFile = new FileOutputStream("GZIPOutClose2.txt");
			TestGZIPOutputStream outGZIP = new TestGZIPOutputStream(outFile);
			outGZIP.close();
			int r = 0;
			try {
				outGZIP.write(byteArray, 0, 1);
			} catch (IOException e) {
				r = 1;
			}
			assertEquals("GZIP instance can still be used after close is called",
					1, r);
		} catch (IOException e) {
			fail(
					"an IO error occured while trying to find the output file or creating GZIP constructor");
		}
	}

	/**
	 * @tests java.util.zip.GZIPOutputStream#write(byte[], int, int)
	 */
	public void test_write$BII() {
		// test method java.util.zip.GZIPOutputStream.writeBII
		byte byteArray[] = { 3, 5, 2, 'r', 'g', 'e', 'f', 'd', 'e', 'w' };
		try {
			FileOutputStream outFile = new FileOutputStream("GZIPOutWrite.txt");
			TestGZIPOutputStream outGZIP = new TestGZIPOutputStream(outFile);
			outGZIP.write(byteArray, 0, 10);
			// ran JDK and found this CRC32 value is 3097700292
			// System.out.print(outGZIP.getChecksum().getValue());
			assertEquals("the checksum value was incorrect result of write from GZIP",
					3097700292L, outGZIP.getChecksum().getValue());

			// test for boundary check
			int r = 0;
			try {
				outGZIP.write(byteArray, 0, 11);
			} catch (ArrayIndexOutOfBoundsException e) {
				r = 1;
			}
			assertEquals("out of bounds exception is not present", 1, r);
			outGZIP.close();
		} catch (IOException e) {
			fail(
					"an IO error occured while trying to find the output file or creating GZIP constructor");
		}
	}

	@Override
    protected void setUp() {
	}

	@Override
    protected void tearDown() {

		try {
			File dFile = new File("GZIPOutCon.txt");
			dFile.delete();
			File dFile2 = new File("GZIPOutFinish.txt"); 
            dFile2.delete();
            File dFile3 = new File("GZIPOutWrite.txt"); 
            dFile3.delete();
            File dFile4 = new File("GZIPOutClose2.txt"); 
            dFile4.delete(); 
		} catch (SecurityException e) {
			fail("Cannot delete file for security reasons");		
		}
	}

}
