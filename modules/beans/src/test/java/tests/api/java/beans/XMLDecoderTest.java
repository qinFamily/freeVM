/* Copyright 2005 The Apache Software Foundation or its licensors, as applicable
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

package tests.api.java.beans;

import java.beans.ExceptionListener;
import java.beans.XMLDecoder;
import java.beans.XMLEncoder;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

import junit.framework.TestCase;
import tests.api.java.beans.EncoderTest.SampleBean;
import tests.api.java.beans.XMLEncoderTest.DependencyBean;
import tests.api.java.beans.mock.MockBean4Codec;
import tests.api.java.beans.mock.MockExceptionListener;

/**
 * Tests XMLDecoder
 */
public class XMLDecoderTest extends TestCase {

	public static void main(String[] args) {
		junit.textui.TestRunner.run(XMLDecoderTest.class);
	}

	static byte xml123bytes[] = null;

	static {
		ByteArrayOutputStream byteout = new ByteArrayOutputStream();
		XMLEncoder enc = new XMLEncoder(byteout);
		enc.writeObject(Integer.valueOf("1"));
		enc.writeObject(Integer.valueOf("2"));
		enc.writeObject(Integer.valueOf("3"));
		enc.close();
		xml123bytes = byteout.toByteArray();
	}

	public void testClose() {
		XMLDecoder dec = new XMLDecoder(new ByteArrayInputStream(xml123bytes));
		assertEquals(Integer.valueOf("1"), dec.readObject());

		dec.close();

		assertEquals(Integer.valueOf("2"), dec.readObject());
		assertEquals(Integer.valueOf("3"), dec.readObject());
	}

	public void testGetExceptionListener() {
		XMLDecoder dec = new XMLDecoder(new ByteArrayInputStream(xml123bytes));
		assertNotNull(dec.getExceptionListener());
	}

	public void testGetOwner() {
		XMLDecoder dec = new XMLDecoder(new ByteArrayInputStream(xml123bytes));
		assertNull(dec.getOwner());
	}

	public void testReadObject_ArrayOutOfBounds() {
		XMLDecoder dec = new XMLDecoder(new ByteArrayInputStream(xml123bytes));
		assertEquals(Integer.valueOf("1"), dec.readObject());
		assertEquals(Integer.valueOf("2"), dec.readObject());
		assertEquals(Integer.valueOf("3"), dec.readObject());

		try {
			dec.readObject();
			fail();
		} catch (ArrayIndexOutOfBoundsException e) {
			// expected
		}
	}

	public void testReadObject_Null() {
		XMLDecoder dec = new XMLDecoder(this.getClass().getResourceAsStream(
				"null.xml"));
		Object obj = dec.readObject();
		assertNull(obj);
	}

	public void testReadObject_Integer() {
		XMLDecoder dec = new XMLDecoder(this.getClass().getResourceAsStream(
				"int.xml"));
		Object obj = dec.readObject();
		assertEquals(Integer.valueOf("3"), obj);
	}

	public void testReadObject_StringCodec() {
		XMLDecoder dec = new XMLDecoder(this.getClass().getResourceAsStream(
				"SampleBean_StringCodec.xml"));
		SampleBean obj = (SampleBean) dec.readObject();
		assertEquals("<Li Yang> & \"liyang'", obj.getMyid());
		assertEquals("a child", obj.getRef().getMyid());
	}

	public void testReadObject_IntArray() {
		XMLDecoder dec = new XMLDecoder(this.getClass().getResourceAsStream(
				"IntArray.xml"));
		int ints[] = (int[]) dec.readObject();
		assertEquals(1, ints[0]);
		assertEquals(2, ints[1]);
		assertEquals(3, ints[2]);
	}

	public void testReadObject_PropertyDependency() {
		XMLDecoder dec = new XMLDecoder(this.getClass().getResourceAsStream(
				"DependencyBean.xml"));
		DependencyBean b = (DependencyBean) dec.readObject();
		assertEquals(888, b.getInts()[0]);
		assertSame(b.getInts(), b.getRef());
	}

	public void testReadObject_NoChange() {
		XMLDecoder dec = new XMLDecoder(this.getClass().getResourceAsStream(
				"MockBean4Codec_NoChange.xml"));
		MockBean4Codec b = (MockBean4Codec) dec.readObject();
	}

	public void testReadObject_BornFriendChange() {
		XMLDecoder dec = new XMLDecoder(this.getClass().getResourceAsStream(
				"MockBean4Codec_BornFriendChange.xml"));
		MockBean4Codec b = (MockBean4Codec) dec.readObject();
		assertEquals(888, b.getBornFriend().getZarr()[0]);
		assertEquals(b.getBornFriend(), b.getNill());
	}

	public void testReadObject_ManyChanges() {
		XMLDecoder dec = new XMLDecoder(this.getClass().getResourceAsStream(
				"MockBean4Codec_ManyChanges.xml"));
		MockBean4Codec b = (MockBean4Codec) dec.readObject();
		assertEquals(127, b.getB());
		assertSame(b, b.getBackRef());
		assertEquals(new Byte((byte) 127), b.getBobj());
		assertEquals(false, b.isBool());
		assertEquals(Boolean.TRUE, b.getBoolobj());
		assertEquals(Exception.class, b.getBornFriend().getClazz());
		assertEquals(888, b.getBornFriend().getZarr()[0]);
		assertEquals('Z', b.getC());
		assertEquals(String.class, b.getClazz());
		assertEquals(new Character('z'), b.getCobj());
		assertEquals(123.456, b.getD(), 0);
		assertEquals(new Double(123.456), b.getDobj());
		assertEquals(12.34F, b.getF(), 0);
		assertEquals(new Float(12.34F), b.getFobj());
		assertEquals(MockBean4Codec.class, b.getFriend().getClazz());
		assertEquals(999, b.getI());
		assertEquals(new Integer(999), b.getIobj());
		assertEquals(8888888, b.getL());
		assertEquals(new Long(8888888), b.getLobj());
		assertEquals("Li Yang", b.getName());
		assertNull(b.getNill());
		assertEquals(55, b.getS());
		assertEquals(new Short((short) 55), b.getSobj());
		assertEquals(3, b.getZarr().length);
		assertEquals(3, b.getZarr()[0]);
		assertEquals(2, b.getZarr()[1]);
		assertEquals(1, b.getZarr()[2]);
		assertEquals(1, b.getZarrarr().length);
		assertEquals(3, b.getZarrarr()[0].length);
		assertEquals("6", b.getZarrarr()[0][0]);
		assertEquals("6", b.getZarrarr()[0][1]);
		assertEquals("6", b.getZarrarr()[0][2]);
	}

	public void testReadObject_StaticField() {
		XMLDecoder dec1 = new XMLDecoder(this.getClass().getResourceAsStream(
				"MockBean4StaticField_Original.xml"));
		MockBean4StaticField o1 = (MockBean4StaticField) dec1.readObject();

		XMLDecoder dec2 = new XMLDecoder(this.getClass().getResourceAsStream(
				"MockBean4StaticField.xml"));
		MockBean4StaticField o2 = (MockBean4StaticField) dec2.readObject();

		if (!o1.equals(o2)) {
			System.out
					.println("Loading object with static field, original xml: "
							+ o1.getV());
			System.out.println("Loading object with static field, field xml: "
					+ o2.getV());
		}

		assertEquals(o1, o2);
	}

	public void testReadObject_Owner() {
		MockBean4Owner_Owner o1 = new MockBean4Owner_Owner();
		XMLDecoder dec1 = new XMLDecoder(this.getClass().getResourceAsStream(
				"MockBean4Owner_SetOwner.xml"), o1);
		MockBean4Owner_Target t1 = (MockBean4Owner_Target) dec1.readObject();

		assertEquals(o1.getV(), 1);
		assertEquals(o1, t1.getV());
	}

	public void testReadObject_Owner_WithWriteStatement() {
		MockBean4Owner_Owner o2 = new MockBean4Owner_Owner();
		XMLDecoder dec2 = new XMLDecoder(this.getClass().getResourceAsStream(
				"MockBean4Owner_SetOwnerWithWriteStatement.xml"), o2);
		MockBean4Owner_Target t2 = (MockBean4Owner_Target) dec2.readObject();

		assertEquals(o2.getV(), 999);
		assertEquals(o2, t2.getV());
	}

	public void testSetExceptionListener() {
		XMLDecoder dec = new XMLDecoder(new ByteArrayInputStream(xml123bytes));
		Object defaultL = dec.getExceptionListener();

		dec.setExceptionListener(null);
		assertSame(defaultL, dec.getExceptionListener());

		ExceptionListener newL = new MockExceptionListener();
		dec.setExceptionListener(newL);
		assertSame(newL, dec.getExceptionListener());
	}

	public void testSetExceptionListener_CatchException() {
		MockExceptionListener l = new MockExceptionListener();
		XMLDecoder dec = new XMLDecoder(XMLDecoderTest.class
				.getResourceAsStream("bad_int.xml"), null, l);
		assertTrue(l.size() > 0);
	}

	public void testSetOwner() {
		XMLDecoder dec = new XMLDecoder(new ByteArrayInputStream(xml123bytes));
		assertNull(dec.getOwner());

		String owner = "owner";
		dec.setOwner(owner);
		assertSame(owner, dec.getOwner());

		dec.setOwner(null);
		assertNull(dec.getOwner());
	}

	/*
	 * Class under test for void XMLDecoder(java.io.InputStream)
	 */
	public void testXMLDecoderInputStream() {
		XMLDecoder dec = new XMLDecoder(new ByteArrayInputStream(xml123bytes));
		assertNull(dec.getOwner());
		assertNotNull(dec.getExceptionListener());
	}

	/*
	 * Class under test for void XMLDecoder(java.io.InputStream,
	 * java.lang.Object)
	 */
	public void testXMLDecoderInputStreamObject() {
		String owner = "owner";
		XMLDecoder dec = new XMLDecoder(new ByteArrayInputStream(xml123bytes),
				owner);
		assertSame(owner, dec.getOwner());
		assertNotNull(dec.getExceptionListener());
	}

	/*
	 * Class under test for void XMLDecoder(java.io.InputStream,
	 * java.lang.Object, java.beans.ExceptionListener)
	 */
	public void testXMLDecoderInputStreamObjectExceptionListener() {
		String owner = "owner";
		MockExceptionListener l = new MockExceptionListener();
		XMLDecoder dec = new XMLDecoder(new ByteArrayInputStream(xml123bytes),
				owner, l);
		assertSame(owner, dec.getOwner());
		assertSame(l, dec.getExceptionListener());
	}

}
