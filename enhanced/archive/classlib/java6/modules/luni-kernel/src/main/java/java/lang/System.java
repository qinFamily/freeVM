/* 
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package java.lang;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.nio.channels.Channel;
import java.nio.channels.spi.SelectorProvider;
import java.security.Policy;
import java.util.Map;
import java.util.Properties;
import java.util.PropertyPermission;

/**
 * Class System provides a standard place for programs to find system related
 * information. All System API is static.
 * 
 */
public final class System {

    // The standard input, output, and error streams.
    // Typically, these are connected to the shell which
    // ran the Java program.
    /**
     * Default input stream
     */
    public static final InputStream in;

    /**
     * Default output stream
     */
    public static final PrintStream out;

    /**
     * Default error output stream
     */
    public static final PrintStream err;

    // Get a ref to the Runtime instance for faster lookup
    private static final Runtime RUNTIME = Runtime.getRuntime();

    // The System Properties table
    private static Properties systemProperties;

    // The System default SecurityManager
    private static SecurityManager security;

    // Indicates whether the classes needed for
    // permission checks was initialized or not
    private static boolean security_initialized;

    // Initialize all the slots in System on first use.
    static {
        // Fill in the properties from the VM information.
        ensureProperties();
        // Set up standard in, out, and err.
        err = new String.ConsolePrintStream(new BufferedOutputStream(new FileOutputStream(
                FileDescriptor.err)));
        out = new String.ConsolePrintStream(new BufferedOutputStream(new FileOutputStream(
                FileDescriptor.out)));
        in = new BufferedInputStream(new FileInputStream(FileDescriptor.in));
    }

    /**
     * Sets the value of the static slot "in" in the receiver to the passed in
     * argument.
     * 
     * @param newIn the new value for in.
     */
    @SuppressWarnings("unused")
    public static void setIn(InputStream newIn) {
        SecurityManager secMgr = System.getSecurityManager();
        setFieldImpl("in", newIn);
    }

    /**
     * Sets the value of the static slot "out" in the receiver to the passed in
     * argument.
     * 
     * @param newOut the new value for out.
     */
    @SuppressWarnings("unused")
    public static void setOut(java.io.PrintStream newOut) {
        SecurityManager secMgr = System.getSecurityManager();
        setFieldImpl("out", newOut);
    }

    /**
     * Sets the value of the static slot "err" in the receiver to the passed in
     * argument.
     * 
     * @param newErr the new value for err.
     */
    @SuppressWarnings("unused")
    public static void setErr(java.io.PrintStream newErr) {
        SecurityManager secMgr = System.getSecurityManager();
    }

    /**
     * Prevents this class from being instantiated.
     */
    private System() {
    }

    /**
     * Copies the contents of <code>array1</code> starting at offset
     * <code>start1</code> into <code>array2</code> starting at offset
     * <code>start2</code> for <code>length</code> elements.
     * 
     * @param array1 the array to copy out of
     * @param start1 the starting index in array1
     * @param array2 the array to copy into
     * @param start2 the starting index in array2
     * @param length the number of elements in the array to copy
     */
    public static void arraycopy(Object array1, int start1, Object array2, int start2,
            int length) {
        // sending getClass() to both arguments will check for null
        Class<?> type1 = array1.getClass();
        Class<?> type2 = array2.getClass();
        if (!type1.isArray() || !type2.isArray()) {
            throw new ArrayStoreException();
        }
        Class<?> componentType1 = type1.getComponentType();
        Class<?> componentType2 = type2.getComponentType();
        if (!componentType1.isPrimitive()) {
            if (componentType2.isPrimitive()) {
                throw new ArrayStoreException();
            }
            arraycopy((Object[]) array1, start1, (Object[]) array2, start2, length);
        } else {
            if (componentType2 != componentType1) {
                throw new ArrayStoreException();
            }
            if (componentType1 == Integer.TYPE) {
                arraycopy((int[]) array1, start1, (int[]) array2, start2, length);
            } else if (componentType1 == Byte.TYPE) {
                arraycopy((byte[]) array1, start1, (byte[]) array2, start2, length);
            } else if (componentType1 == Long.TYPE) {
                arraycopy((long[]) array1, start1, (long[]) array2, start2, length);
            } else if (componentType1 == Short.TYPE) {
                arraycopy((short[]) array1, start1, (short[]) array2, start2, length);
            } else if (componentType1 == Character.TYPE) {
                arraycopy((char[]) array1, start1, (char[]) array2, start2, length);
            } else if (componentType1 == Boolean.TYPE) {
                arraycopy((boolean[]) array1, start1, (boolean[]) array2, start2, length);
            } else if (componentType1 == Double.TYPE) {
                arraycopy((double[]) array1, start1, (double[]) array2, start2, length);
            } else if (componentType1 == Float.TYPE) {
                arraycopy((float[]) array1, start1, (float[]) array2, start2, length);
            }
        }
    }

    /**
     * Private version of the arraycopy method (used by the jit for reference
     * arraycopies)
     */
    private static void arraycopy(Object[] A1, int offset1, Object[] A2, int offset2, int length) {
        if (offset1 >= 0 && offset2 >= 0 && length >= 0 && length <= A1.length - offset1
                && length <= A2.length - offset2) {
            // Check if this is a forward or backwards arraycopy
            if (A1 != A2 || offset1 > offset2 || offset1 + length <= offset2) {
                for (int i = 0; i < length; ++i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            } else {
                for (int i = length - 1; i >= 0; --i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            }
        } else {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    /**
     * Copies the contents of <code>A1</code> starting at offset
     * <code>offset1</code> into <code>A2</code> starting at offset
     * <code>offset2</code> for <code>length</code> elements.
     * 
     * @param A1 the array to copy out of
     * @param offset1 the starting index in array1
     * @param A2 the array to copy into
     * @param offset2 the starting index in array2
     * @param length the number of elements in the array to copy
     */
    private static void arraycopy(int[] A1, int offset1, int[] A2, int offset2, int length) {
        if (offset1 >= 0 && offset2 >= 0 && length >= 0 && length <= A1.length - offset1
                && length <= A2.length - offset2) {
            // Check if this is a forward or backwards arraycopy
            if (A1 != A2 || offset1 > offset2 || offset1 + length <= offset2) {
                for (int i = 0; i < length; ++i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            } else {
                for (int i = length - 1; i >= 0; --i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            }
        } else {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    /**
     * Copies the contents of <code>A1</code> starting at offset
     * <code>offset1</code> into <code>A2</code> starting at offset
     * <code>offset2</code> for <code>length</code> elements.
     * 
     * @param A1 the array to copy out of
     * @param offset1 the starting index in array1
     * @param A2 the array to copy into
     * @param offset2 the starting index in array2
     * @param length the number of elements in the array to copy
     */
    private static void arraycopy(byte[] A1, int offset1, byte[] A2, int offset2, int length) {
        if (offset1 >= 0 && offset2 >= 0 && length >= 0 && length <= A1.length - offset1
                && length <= A2.length - offset2) {
            // Check if this is a forward or backwards arraycopy
            if (A1 != A2 || offset1 > offset2 || offset1 + length <= offset2) {
                for (int i = 0; i < length; ++i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            } else {
                for (int i = length - 1; i >= 0; --i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            }
        } else {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    /**
     * Copies the contents of <code>A1</code> starting at offset
     * <code>offset1</code> into <code>A2</code> starting at offset
     * <code>offset2</code> for <code>length</code> elements.
     * 
     * @param A1 the array to copy out of
     * @param offset1 the starting index in array1
     * @param A2 the array to copy into
     * @param offset2 the starting index in array2
     * @param length the number of elements in the array to copy
     */
    private static void arraycopy(short[] A1, int offset1, short[] A2, int offset2, int length) {
        if (offset1 >= 0 && offset2 >= 0 && length >= 0 && length <= A1.length - offset1
                && length <= A2.length - offset2) {
            // Check if this is a forward or backwards arraycopy
            if (A1 != A2 || offset1 > offset2 || offset1 + length <= offset2) {
                for (int i = 0; i < length; ++i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            } else {
                for (int i = length - 1; i >= 0; --i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            }
        } else {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    /**
     * Copies the contents of <code>A1</code> starting at offset
     * <code>offset1</code> into <code>A2</code> starting at offset
     * <code>offset2</code> for <code>length</code> elements.
     * 
     * @param A1 the array to copy out of
     * @param offset1 the starting index in array1
     * @param A2 the array to copy into
     * @param offset2 the starting index in array2
     * @param length the number of elements in the array to copy
     */
    private static void arraycopy(long[] A1, int offset1, long[] A2, int offset2, int length) {
        if (offset1 >= 0 && offset2 >= 0 && length >= 0 && length <= A1.length - offset1
                && length <= A2.length - offset2) {
            // Check if this is a forward or backwards arraycopy
            if (A1 != A2 || offset1 > offset2 || offset1 + length <= offset2) {
                for (int i = 0; i < length; ++i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            } else {
                for (int i = length - 1; i >= 0; --i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            }
        } else {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    /**
     * Copies the contents of <code>A1</code> starting at offset
     * <code>offset1</code> into <code>A2</code> starting at offset
     * <code>offset2</code> for <code>length</code> elements.
     * 
     * @param A1 the array to copy out of
     * @param offset1 the starting index in array1
     * @param A2 the array to copy into
     * @param offset2 the starting index in array2
     * @param length the number of elements in the array to copy
     */
    private static void arraycopy(char[] A1, int offset1, char[] A2, int offset2, int length) {
        if (offset1 >= 0 && offset2 >= 0 && length >= 0 && length <= A1.length - offset1
                && length <= A2.length - offset2) {
            // Check if this is a forward or backwards arraycopy
            if (A1 != A2 || offset1 > offset2 || offset1 + length <= offset2) {
                for (int i = 0; i < length; ++i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            } else {
                for (int i = length - 1; i >= 0; --i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            }
        } else {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    /**
     * Copies the contents of <code>A1</code> starting at offset
     * <code>offset1</code> into <code>A2</code> starting at offset
     * <code>offset2</code> for <code>length</code> elements.
     * 
     * @param A1 the array to copy out of
     * @param offset1 the starting index in array1
     * @param A2 the array to copy into
     * @param offset2 the starting index in array2
     * @param length the number of elements in the array to copy
     */
    private static void arraycopy(boolean[] A1, int offset1, boolean[] A2, int offset2,
            int length) {
        if (offset1 >= 0 && offset2 >= 0 && length >= 0 && length <= A1.length - offset1
                && length <= A2.length - offset2) {
            // Check if this is a forward or backwards arraycopy
            if (A1 != A2 || offset1 > offset2 || offset1 + length <= offset2) {
                for (int i = 0; i < length; ++i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            } else {
                for (int i = length - 1; i >= 0; --i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            }
        } else {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    /**
     * Copies the contents of <code>A1</code> starting at offset
     * <code>offset1</code> into <code>A2</code> starting at offset
     * <code>offset2</code> for <code>length</code> elements.
     * 
     * @param A1 the array to copy out of
     * @param offset1 the starting index in array1
     * @param A2 the array to copy into
     * @param offset2 the starting index in array2
     * @param length the number of elements in the array to copy
     */
    private static void arraycopy(double[] A1, int offset1, double[] A2, int offset2, int length) {
        if (offset1 >= 0 && offset2 >= 0 && length >= 0 && length <= A1.length - offset1
                && length <= A2.length - offset2) {
            // Check if this is a forward or backwards arraycopy
            if (A1 != A2 || offset1 > offset2 || offset1 + length <= offset2) {
                for (int i = 0; i < length; ++i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            } else {
                for (int i = length - 1; i >= 0; --i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            }
        } else {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    /**
     * Copies the contents of <code>A1</code> starting at offset
     * <code>offset1</code> into <code>A2</code> starting at offset
     * <code>offset2</code> for <code>length</code> elements.
     * 
     * @param A1 the array to copy out of
     * @param offset1 the starting index in array1
     * @param A2 the array to copy into
     * @param offset2 the starting index in array2
     * @param length the number of elements in the array to copy
     */
    private static void arraycopy(float[] A1, int offset1, float[] A2, int offset2, int length) {
        if (offset1 >= 0 && offset2 >= 0 && length >= 0 && length <= A1.length - offset1
                && length <= A2.length - offset2) {
            // Check if this is a forward or backwards arraycopy
            if (A1 != A2 || offset1 > offset2 || offset1 + length <= offset2) {
                for (int i = 0; i < length; ++i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            } else {
                for (int i = length - 1; i >= 0; --i) {
                    A2[offset2 + i] = A1[offset1 + i];
                }
            }
        } else {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    /**
     * Answers the current time expressed as milliseconds since the time
     * 00:00:00 UTC on January 1, 1970.
     * 
     * @return the time in milliseconds.
     */
    public static native long currentTimeMillis();

    /**
     * <p>
     * Returns the most precise time measurement in nanoseconds that's
     * available.
     * </p>
     * 
     * @return The current time in nanoseconds.
     */
    public static native long nanoTime();

    private static final int InitLocale = 0;

    private static final int PlatformEncoding = 1;

    private static final int FileEncoding = 2;

    private static final int OSEncoding = 3;

    /**
     * If systemProperties is unset, then create a new one based on the values
     * provided by the virtual machine.
     */
    private static void ensureProperties() {
        systemProperties = new Properties();

        String platformEncoding = null;
        String fileEncoding, osEncoding = null;
        String definedFileEncoding = getEncoding(FileEncoding);
        String definedOSEncoding = getEncoding(OSEncoding);
        if (definedFileEncoding != null) {
            fileEncoding = definedFileEncoding;
            // if file.encoding is defined, and os.encoding is not, use the
            // detected
            // platform encoding for os.encoding
            if (definedOSEncoding == null) {
                platformEncoding = getEncoding(PlatformEncoding);
                osEncoding = platformEncoding;
            } else {
                getEncoding(InitLocale);
            }
        } else {
            platformEncoding = getEncoding(PlatformEncoding);
            fileEncoding = platformEncoding;
        }
        // if os.encoding is not defined, file.encoding will be used
        if (osEncoding == null) {
            osEncoding = definedOSEncoding;
        }
        if (osEncoding != null) {
            systemProperties.put("os.encoding", osEncoding);
        }

        systemProperties.put("file.encoding", fileEncoding);

        systemProperties.put("java.version", "1.4.2 subset");
        systemProperties.put("java.specification.version", "1.4");

        systemProperties.put("java.specification.vendor", "Sun Microsystems Inc.");
        systemProperties.put("java.specification.name", "Java Platform API Specification");

        systemProperties.put("com.ibm.oti.configuration", "clear");
        systemProperties.put("com.ibm.oti.configuration.dir", "jclClear");

        String[] list = getPropertyList();
        for (int i = 0; i < list.length; i += 2) {
            String key = list[i];
            if (key == null) {
                break;
            }
            systemProperties.put(key, list[i + 1]);
        }

        String consoleEncoding = (String) systemProperties.get("console.encoding");
        if (consoleEncoding == null) {
            if (platformEncoding == null) {
                platformEncoding = getEncoding(PlatformEncoding);
            }
            consoleEncoding = platformEncoding;
            systemProperties.put("console.encoding", consoleEncoding);
        }

    }

    /**
     * Causes the virtual machine to stop running, and the program to exit. If
     * runFinalizersOnExit(true) has been invoked, then all finalizers will be
     * run first.
     * 
     * @param code the return code.
     * 
     * @throws SecurityException if the running thread is not allowed to cause
     *         the vm to exit.
     * 
     * @see SecurityManager#checkExit
     */
    public static void exit(int code) {
        RUNTIME.exit(code);
    }

    /**
     * Indicate to the virtual machine that it would be a good time to collect
     * available memory. Note that, this is a hint only.
     */
    public static void gc() {
        RUNTIME.gc();
    }

    /**
     * Returns an environment variable.
     * 
     * @param var the name of the environment variable
     * @return the value of the specified environment variable
     */
    public static String getenv(String var) {
        if (var == null) {
            throw new NullPointerException();
        }
        SecurityManager secMgr = System.getSecurityManager();
        if (secMgr != null) {
            secMgr.checkPermission(new RuntimePermission("getenv." + var));
        }
        throw new Error();
    }

    /**
     * <p>
     * Returns all environment variables.
     * </p>
     * 
     * @return A Map of all environment variables.
     */
    public static Map<String, String> getenv() {
        SecurityManager secMgr = System.getSecurityManager();
        if (secMgr != null) {
            secMgr.checkPermission(new RuntimePermission("getenv.*"));
        }
        throw new Error();
    }

    /**
     * <p>
     * Returns the inherited channel from the system-wide provider.
     * </p>
     * 
     * @return A {@link Channel} or <code>null</code>.
     * @throws IOException
     * @see SelectorProvider
     * @see SelectorProvider#inheritedChannel()
     */
    public static Channel inheritedChannel() throws IOException {
        return SelectorProvider.provider().inheritedChannel();
    }

    /**
     * Answers the system properties. Note that this is not a copy, so that
     * changes made to the returned Properties object will be reflected in
     * subsequent calls to getProperty and getProperties.
     * <p>
     * Security managers should restrict access to this API if possible.
     * 
     * @return the system properties
     */
    public static Properties getProperties() {
        SecurityManager secMgr = System.getSecurityManager();
        if (secMgr != null) {
            secMgr.checkPropertiesAccess();
        }
        return systemProperties;
    }

    /**
     * Answers the system properties without any security checks. This is used
     * for access from within java.lang.
     * 
     * @return the system properties
     */
    static Properties internalGetProperties() {
        return systemProperties;
    }

    /**
     * Answers the value of a particular system property. Answers null if no
     * such property exists,
     * <p>
     * The properties currently provided by the virtual machine are:
     * 
     * <pre>
     *        java.vendor.url
     *        java.class.path
     *        user.home
     *        java.class.version
     *        os.version
     *        java.vendor
     *        user.dir
     *        user.timezone
     *        path.separator
     *        os.name
     *        os.arch
     *        line.separator
     *        file.separator
     *        user.name
     *        java.version
     *        java.home
     * </pre>
     * 
     * @param prop the system property to look up
     * @return the value of the specified system property, or null if the
     *         property doesn't exist
     */
    public static String getProperty(String prop) {
        return getProperty(prop, null);
    }

    /**
     * Answers the value of a particular system property. If no such property is
     * found, answers the defaultValue.
     * 
     * @param prop the system property to look up
     * @param defaultValue return value if system property is not found
     * @return the value of the specified system property, or defaultValue if
     *         the property doesn't exist
     */
    public static String getProperty(String prop, String defaultValue) {
        if (prop.length() == 0) {
            throw new IllegalArgumentException();
        }
        SecurityManager secMgr = System.getSecurityManager();
        if (secMgr != null) {
            secMgr.checkPropertyAccess(prop);
        }
        return systemProperties.getProperty(prop, defaultValue);
    }

    /**
     * Sets the value of a particular system property.
     * 
     * @param prop the system property to change
     * @param value the value to associate with prop
     * @return the old value of the property, or null
     */
    public static String setProperty(String prop, String value) {
        if (prop.length() == 0) {
            throw new IllegalArgumentException();
        }
        SecurityManager secMgr = System.getSecurityManager();
        if (secMgr != null) {
            secMgr.checkPermission(new PropertyPermission(prop, "write"));
        }
        return (String) systemProperties.setProperty(prop, value);
    }

    /**
     * <p>
     * Removes the system property for the specified key.
     * </p>
     * 
     * <p>
     * Please see the Java SE API documentation for further
     * information on this method.
     * <p>
     * 
     * @param key the system property to be removed.
     * @return previous value or null if no value existed
     * 
     * @throws NullPointerException if the <code>key</code> argument is
     *         <code>null</code>.
     * @throws IllegalArgumentException if the <code>key</code> argument is
     *         empty.
     * @throws SecurityException if a security manager exists and write access
     *         to the specified property is not allowed.
     * @since 1.5
     */
    public static String clearProperty(String key) {
        if (key == null) {
            throw new NullPointerException();
        }
        if (key.length() == 0) {
            throw new IllegalArgumentException();
        }

        SecurityManager secMgr = System.getSecurityManager();
        if (secMgr != null) {
            secMgr.checkPermission(new PropertyPermission(key, "write"));
        }
        return (String) systemProperties.remove(key);
    }

    /**
     * Answers an array of Strings containing key..value pairs (in consecutive
     * array elements) which represent the starting values for the system
     * properties as provided by the virtual machine.
     * 
     * @return the default values for the system properties.
     */
    private static native String[] getPropertyList();

    /**
     * Return the requested encoding. 0 - initialize locale 1 - detected
     * platform encoding 2 - command line defined file.encoding 3 - command line
     * defined os.encoding
     */
    private static native String getEncoding(int type);

    /**
     * Answers the active security manager.
     * 
     * @return the system security manager object.
     */
    public static SecurityManager getSecurityManager() {
        return security;
    }

    /**
     * Answers an integer hash code for the parameter. The hash code returned is
     * the same one that would be returned by java.lang.Object.hashCode(),
     * whether or not the object's class has overridden hashCode(). The hash
     * code for null is 0.
     * 
     * @param anObject the object
     * @return the hash code for the object
     * 
     * @see java.lang.Object#hashCode
     */
    public static native int identityHashCode(Object anObject);

    /**
     * Loads the specified file as a dynamic library.
     * 
     * @param pathName the path of the file to be loaded
     */
    public static void load(String pathName) {
        SecurityManager smngr = System.getSecurityManager();
        if (smngr != null) {
            smngr.checkLink(pathName);
        }
        ClassLoader.loadLibraryWithPath(pathName, ClassLoader.callerClassLoader(), null);
    }

    /**
     * Loads and links the library specified by the argument.
     * 
     * @param libName the name of the library to load
     * 
     * @throws UnsatisfiedLinkError if the library could not be loaded
     * @throws SecurityException if the library was not allowed to be loaded
     */
    public static void loadLibrary(String libName) {
        ClassLoader.loadLibraryWithClassLoader(libName, ClassLoader.callerClassLoader());
    }

    /**
     * Provides a hint to the virtual machine that it would be useful to attempt
     * to perform any outstanding object finalizations.
     */
    public static void runFinalization() {
        RUNTIME.runFinalization();
    }

    /**
     * Ensure that, when the virtual machine is about to exit, all objects are
     * finalized. Note that all finalization which occurs when the system is
     * exiting is performed after all running threads have been terminated.
     * 
     * @param flag
     *            true means finalize all on exit.
     * 
     * @deprecated This method is unsafe.
     */
    @SuppressWarnings("deprecation")
    @Deprecated
    public static void runFinalizersOnExit(boolean flag) {
        Runtime.runFinalizersOnExit(flag);
    }

    /**
     * Answers the system properties. Note that the object which is passed in
     * not copied, so that subsequent changes made to the object will be
     * reflected in calls to getProperty and getProperties.
     * <p>
     * Security managers should restrict access to this API if possible.
     * 
     * @param p
     *            the property to set
     */
    public static void setProperties(Properties p) {
        SecurityManager secMgr = System.getSecurityManager();
        if (secMgr != null) {
            secMgr.checkPropertiesAccess();
        }
        if (p == null) {
            ensureProperties();
        } else {
            systemProperties = p;
        }
    }

    /**
     * Sets the active security manager. Note that once the security manager has
     * been set, it can not be changed. Attempts to do so will cause a security
     * exception.
     * 
     * @param s
     *            the new security manager
     * 
     * @throws SecurityException
     *             if the security manager has already been set.
     */
    public static void setSecurityManager(final SecurityManager s) {
        if (!security_initialized) {
            try {
                // Preload and initialize Policy implementation classes
                // otherwise we could go recursive
                Policy.getPolicy();
            } catch (Exception e) {
            }
            security_initialized = true;
        }

        security = s;
    }

    /**
     * Answers the platform specific file name format for the shared library
     * named by the argument.
     * 
     * @param userLibName
     *            the name of the library to look up.
     * @return the platform specific filename for the library
     */
    public static native String mapLibraryName(String userLibName);

    /**
     * Sets the value of the named static field in the receiver to the passed in
     * argument.
     * 
     * @param fieldName
     *            the name of the field to set, one of in, out, or err
     * @param stream
     *            the new value of the field
     */
    private static native void setFieldImpl(String fieldName, Object stream);

}
