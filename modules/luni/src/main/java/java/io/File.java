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

package java.io;


import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.security.AccessController;

import com.ibm.oti.util.PriviAction;

/**
 * File is a class which represents a file name or directory. The file may be
 * absolute relative to the root directory of the file system or relative to the
 * current directory in which the program is running.
 * <p>
 * This class provides methods for querying/changing information about the file
 * and also directory listing capabilities if the File represents a directory.
 * <p>
 * When manipulating file paths, the static fields of this class may be used to
 * determine the platform specific separators.
 * 
 * @see java.io.Serializable
 * @see java.lang.Comparable
 */

public class File implements Serializable, Comparable {
	static final long serialVersionUID = 301077366599181567L;

	private String path;

	byte[] properPath;

	/**
	 * System dependant file separator character.
	 */
	public static final char separatorChar;

	/**
	 * System dependant file separator String. The initial value of this field
	 * is the System property "file.separator".
	 */
	public static final String separator;

	/**
	 * System dependant path separator character.
	 */
	public static final char pathSeparatorChar;

	/**
	 * System dependant path separator String. The initial value of this field
	 * is the System property "path.separator".
	 */
	public static final String pathSeparator;

	/* Temp file counter needs java.util.Random from mJava */
	private static int counter;

	private static boolean caseSensitive;

	private static native void oneTimeInitialization();

	static {
		oneTimeInitialization();

		// The default protection domain grants access to these properties
		separatorChar = System.getProperty("file.separator", "\\").charAt(0); //$NON-NLS-1$ //$NON-NLS-2$
		pathSeparatorChar = System.getProperty("path.separator", ";").charAt(0);  //$NON-NLS-1$//$NON-NLS-2$
		separator = new String(new char[] { separatorChar }, 0, 1);
		pathSeparator = new String(new char[] { pathSeparatorChar }, 0, 1);
		caseSensitive = isCaseSensitiveImpl();
	}

	/**
	 * Constructs a new File using the specified directory and name.
	 * 
	 * @param dir
	 *            the directory for the file name
	 * @param name
	 *            the file name to be contained in the dir
	 */

	public File(File dir, String name) {
		if (name != null) {
			if (dir == null) {
				this.path = name;
			} else {
				this.path = calculatePath(dir.getPath(),name);
			}
		} else
			throw new NullPointerException();
	}

	/**
	 * Constructs a new File using the specified path.
	 * 
	 * @param path
	 *            the path to be used for the file
	 */
	public File(String path) {
		// NullPointerException thrown by fixSlashes
		this.path = fixSlashes(path);
	}

	/**
	 * Constructs a new File using the specified directory and name placing a
	 * path separator between the two.
	 * 
	 * @param dirPath
	 *            the directory for the file name
	 * @param name
	 *            the file name to be contained in the dir
	 */
	public File(String dirPath, String name) {
		if (name != null) {
			if (dirPath == null) {
				this.path = name;
			} else {
				this.path = calculatePath(dirPath, name);
			}
		} else
			throw new NullPointerException();
	}

	/**
	 * Constructs a new File using the path of the specified URI
	 * 
	 * <code>uri</code> needs to be an absolute and hierarchical
	 * <code>URI </code> with file scheme, and non-empty path component, but
	 * with undefined authority, query or fragment components.
	 * 
	 * @param uri
	 *            the URI instance which will be used to construct this file
	 * 
	 * @throws IllegalArgumentException
	 *             if <code>uri</code> does not comply with the conditions
	 *             above.
	 * 
	 * @see #toURI
	 * @see java.net.URI
	 */
	public File(URI uri) {
		// check pre-conditions
		checkURI(uri);
		this.path = fixSlashes(uri.getPath());
	}

	private String calculatePath(String dirPath, String name) {
		// Remove all the proceeding separator chars from name
		name = fixSlashes(name);
		while (name.length() > 0 && (name.charAt(0) == separatorChar))
			name = name.substring(1, name.length());

		// Ensure there is a separator char between dirPath and name
		dirPath = fixSlashes(dirPath);
		if (dirPath.length() > 0 && (dirPath.charAt(dirPath.length() - 1) == separatorChar)) {
			return dirPath + name;
		}

		return dirPath + separatorChar + name;
	}

	private void checkURI(URI uri) {
		if (!uri.isAbsolute()) {
			throw new IllegalArgumentException(com.ibm.oti.util.Msg.getString(
					"K031a", uri)); //$NON-NLS-1$
		} else if (!uri.getRawSchemeSpecificPart().startsWith("/")) { //$NON-NLS-1$
			throw new IllegalArgumentException(com.ibm.oti.util.Msg.getString(
					"K031b", uri)); //$NON-NLS-1$
		}

		String temp = uri.getScheme();
		if (temp == null || !temp.equals("file")) { //$NON-NLS-1$
			throw new IllegalArgumentException(com.ibm.oti.util.Msg.getString(
					"K031c", uri)); //$NON-NLS-1$
		}

		temp = uri.getRawPath();
		if (temp == null || temp.length() == 0) {
			throw new IllegalArgumentException(com.ibm.oti.util.Msg.getString(
					"K031d", uri)); //$NON-NLS-1$
		}

		if (uri.getRawAuthority() != null) {
			throw new IllegalArgumentException(com.ibm.oti.util.Msg.getString(
					"K031e", new String[] { "authority", uri.toString() })); //$NON-NLS-1$ //$NON-NLS-2$
		}

		if (uri.getRawQuery() != null)
			throw new IllegalArgumentException(com.ibm.oti.util.Msg.getString(
					"K031e", new String[] { "query", uri.toString() }));  //$NON-NLS-1$//$NON-NLS-2$

		if (uri.getRawFragment() != null)
			throw new IllegalArgumentException(com.ibm.oti.util.Msg.getString(
					"K031e", new String[] { "fragment", uri.toString() })); //$NON-NLS-1$ //$NON-NLS-2$
	}

	private static native byte[][] rootsImpl();

	private static native boolean isCaseSensitiveImpl();

	/**
	 * Lists the filesystem roots.
	 * 
	 * The Java platform may support zero or more filesystems, each with its own
	 * platform-dependent root. Further, the canonical pathname of any file on
	 * the system will always begin with one of the returned filesystem roots.
	 * 
	 * @return the array of filesystem roots
	 */
	public static File[] listRoots() {
		byte[][] rootsList = rootsImpl();
		if (rootsList == null)
			return new File[0];
		File result[] = new File[rootsList.length];
		for (int i = 0; i < rootsList.length; i++)
			result[i] = new File(com.ibm.oti.util.Util.toString(rootsList[i]));
		return result;
	}

	/**
	 * The purpose of this method is to take a path and fix the slashes up. This
	 * includes changing them all to the current platforms fileSeparator and
	 * removing duplicates.
	 */
	private String fixSlashes(String origPath) {
		int uncIndex = 1;
		int length = origPath.length(), newLength = 0;
		if (separatorChar == '/') // disable UNC names
			uncIndex = 0;
		else if (length > 2 && origPath.charAt(1) == ':')
			uncIndex = 2;

		boolean foundSlash = false;
		char newPath[] = origPath.toCharArray();
		for (int i = 0; i < length; i++) {
			char pathChar = newPath[i];
			if (pathChar == '\\' || pathChar == '/') {
				/* UNC Name requires 2 leading slashes */
				if ((foundSlash && i == uncIndex) || !foundSlash) {
					newPath[newLength++] = separatorChar;
					foundSlash = true;
				}
			} else {
				// check for leading slashes before a drive
				if (pathChar == ':'
						&& uncIndex > 0
						&& (newLength == 2 || (newLength == 3 && newPath[1] == separatorChar))
						&& newPath[0] == separatorChar) {
					newPath[0] = newPath[newLength - 1];
					newLength = 1;
				}
				newPath[newLength++] = pathChar;
				foundSlash = false;
			}
		}
		// remove trailing slash
		if (foundSlash && newLength > (uncIndex + 1))
			newLength--;
		String tempPath = new String(newPath, 0, newLength);
		// If it's the same keep it identical for SecurityManager purposes
		if (!tempPath.equals(origPath)) {
			return tempPath;
		}
		return origPath;
	}

	/**
	 * Answers a boolean indicating whether or not the current context i allowed
	 * to read this File.
	 * 
	 * @return <code>true</code> if this File can be read, <code>false</code>
	 *         otherwise.
	 * 
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 */
	public boolean canRead() {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkRead(path);
		return exists() && !isWriteOnlyImpl(properPath(true));
	}

	/**
	 * Answers a boolean indicating whether or not the current context is
	 * allowed to write to this File.
	 * 
	 * @return <code>true</code> if this File can be written,
	 *         <code>false</code> otherwise.
	 * 
	 * @see java.lang.SecurityManager#checkWrite(FileDescriptor)
	 */
	public boolean canWrite() {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkWrite(path);
		
		// Cannot use exists() since that does an unwanted read-check.
		boolean exists = false;
		if (path.length() > 0) {
			exists = existsImpl(properPath(true));
		}
		return exists && !isReadOnlyImpl(properPath(true));
	}

	/**
	 * Compare this abstract File with another returning the relative position.
	 * 
	 * @param another
	 *            an object to compare the receiver to
	 * @return an int determined by comparing the two paths.
	 * 
	 * @throws ClassCastException
	 *             if the argument is not a File
	 */
	public int compareTo(Object another) {
		if (this.getClass() == another.getClass())
			return this.getPath().compareTo(((File) another).getPath());
		throw new ClassCastException(com.ibm.oti.util.Msg.getString("K0069")); //$NON-NLS-1$
	}

	/**
	 * Compare this abstract File with another returning the relative position.
	 * 
	 * @param another
	 *            a File to compare the receiver to
	 * @return an int determined by comparing the two paths.
	 */
	public int compareTo(File another) {
		if (caseSensitive) {
			return this.getPath().compareTo(another.getPath());
		}
		return this.getPath().compareToIgnoreCase(another.getPath());

	}

	/**
	 * Deletes the file specified by this File. Directories must be empty before
	 * they will be deleted.
	 * 
	 * @return <code>true</code> if this File was deleted, <code>false</code>
	 *         otherwise.
	 * 
	 * @see java.lang.SecurityManager#checkDelete
	 */
	public boolean delete() {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkDelete(path);
		return isDirectory() ? deleteDirImpl(properPath(true))
				: deleteFileImpl(properPath(true));
	}

	private native boolean deleteDirImpl(byte[] filePath);

	private native boolean deleteFileImpl(byte[] filePath);

	/**
	 * When the virtual machine terminates, any abstract files which have been
	 * sent <code>deleteOnExit()</code> will be deleted. This will only happen
	 * when the virtual machine terminates normally as described by the Java
	 * Language Specification section 12.9.
	 * 
	 */
	public void deleteOnExit() {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkDelete(path);

		com.ibm.oti.util.DeleteOnExit.addFile(com.ibm.oti.util.Util
				.toString(properPath(true)));
	}

	/**
	 * Compares the argument <code>obj</code> to the receiver, and answers
	 * <code>true</code> if they represent the <em>same</em> object using a
	 * path specific comparison.
	 * 
	 * @param obj
	 *            the Object to compare with this Object
	 * @return <code>true</code> if the object is the same as this object,
	 *         <code>false</code> otherwise.
	 */
	public boolean equals(Object obj) {
		if (!(obj instanceof File))
			return false;
		if (!caseSensitive) {
			return path.equalsIgnoreCase(((File) obj).getPath());
		}
		return path.equals(((File) obj).getPath());
	}

	/**
	 * Answers a boolean indicating whether or not this File can be found on the
	 * underlying file system.
	 * 
	 * @return <code>true</code> if this File exists, <code>false</code>
	 *         otherwise.
	 * 
	 * @see #getPath
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 */
	public boolean exists() {
		if (path.length() == 0)
			return false;
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkRead(path);
		return existsImpl(properPath(true));
	}

	private native boolean existsImpl(byte[] filePath);

	/**
	 * Answers the absolute file path of this File.
	 * 
	 * @return the absolute file path
	 * 
	 * @see java.lang.SecurityManager#checkPropertyAccess
	 */
	public String getAbsolutePath() {
		byte[] absolute = properPath(false);
		return com.ibm.oti.util.Util.toString(absolute);
	}

	/**
	 * Answers a new File constructed using the absolute file path of this File.
	 * 
	 * @return a new File from this absolute file path
	 * 
	 * @see java.lang.SecurityManager#checkPropertyAccess
	 */
	public File getAbsoluteFile() {
		return new File(this.getAbsolutePath());
	}

	/**
	 * Answers the absolute file path of this File with all references resolved.
	 * An <em>absolute</em> file path is one which begins at the root of the
	 * file system. The canonical path is one in which all references have been
	 * resolved. For the cases of '..' and '.' where the file system supports
	 * parent and working directory respectively, these should be removed and
	 * replaced with a direct directory reference. If the File does not exist,
	 * getCanonicalPath() may not resolve any references and simply return an
	 * absolute path name or throw an IOException.
	 * 
	 * @return the canonical file path
	 * 
	 * @throws IOException
	 *             if an IO error occurs
	 * 
	 * @see java.lang.SecurityManager#checkPropertyAccess
	 */
	public String getCanonicalPath() throws IOException {
		byte[] result = properPath(false);

		boolean exists = false;
		byte[] pathBytes = result;
		do {
			byte[] linkBytes = getLinkImpl(pathBytes);
			if (linkBytes == pathBytes)
				break;
			if (linkBytes[0] == separatorChar) {
				pathBytes = linkBytes;
			} else {
				int index = pathBytes.length - 1;
				while (pathBytes[index] != separatorChar)
					index--;
				byte[] temp = new byte[index + 1 + linkBytes.length];
				System.arraycopy(pathBytes, 0, temp, 0, index + 1);
				System.arraycopy(linkBytes, 0, temp, index + 1,
						linkBytes.length);
				pathBytes = temp;
			}
			exists = existsImpl(pathBytes);
		} while (exists);
		if (exists)
			result = pathBytes;

		int numSeparators = 1;
		for (int i = 0; i < result.length; i++)
			if (result[i] == separatorChar)
				numSeparators++;
		int sepLocations[] = new int[numSeparators];
		int rootLoc = 0;
		if (separatorChar != '/')
			if (result[0] == '\\')
				rootLoc = (result.length > 1 && result[1] == '\\') ? 1 : 0;
			else
				rootLoc = 2; // skip drive i.e. c:
		byte newResult[] = new byte[result.length + 1];
		int newLength = 0, lastSlash = 0, foundDots = 0;
		sepLocations[lastSlash] = rootLoc;
		for (int i = 0; i <= result.length; i++) {
			if (i < rootLoc)
				newResult[newLength++] = result[i];
			else {
				if (i == result.length || result[i] == separatorChar) {
					if (i == result.length && foundDots == 0)
						break;
					if (foundDots == 1) {
						/* Don't write anything, just reset and continue */
						foundDots = 0;
						continue;
					}
					if (foundDots > 1) {
						/* Go back N levels */
						lastSlash = lastSlash > (foundDots - 1) ? lastSlash
								- (foundDots - 1) : 0;
						newLength = sepLocations[lastSlash] + 1;
						foundDots = 0;
						continue;
					}
					sepLocations[++lastSlash] = newLength;
					newResult[newLength++] = (byte) separatorChar;
					continue;
				}
				if (result[i] == '.') {
					foundDots++;
					continue;
				}
				/* Found some dots within text, write them out */
				if (foundDots > 0)
					for (int j = 0; j < foundDots; j++)
						newResult[newLength++] = (byte) '.';
				newResult[newLength++] = result[i];
				foundDots = 0;
			}
		}
		// remove trailing slash
		if (newLength > (rootLoc + 1)
				&& newResult[newLength - 1] == separatorChar)
			newLength--;
		newResult[newLength] = 0;
		newResult = getCanonImpl(newResult);
		newLength = newResult.length;
		return com.ibm.oti.util.Util.toString(newResult, 0, newLength);
	}

	/**
	 * Answers a new File created using the canonical file path of this File.
	 * Equivalent to <code>new File(this.getCanonicalPath())</code>.
	 * 
	 * @return the canonical file path
	 * 
	 * @throws IOException
	 *             If an IO error occurs
	 * 
	 * @see java.lang.SecurityManager#checkPropertyAccess
	 */
	public File getCanonicalFile() throws IOException {
		return new File(getCanonicalPath());
	}

	private native byte[] getCanonImpl(byte[] filePath);

	/**
	 * Answers the filename (not directory) of this File.
	 * 
	 * @return the filename or empty string
	 */
	public String getName() {
		int separatorIndex = path.lastIndexOf(separator);
		return (separatorIndex < 0) ? path : path.substring(separatorIndex + 1,
				path.length());
	}

	/**
	 * Answers the pathname of the parent of this File. This is the path upto
	 * but not including the last name. <code>null</code> is returned when
	 * there is no parent.
	 * 
	 * @return the parent name or <code>null</code>
	 */
	public String getParent() {
		int length = path.length(), firstInPath = 0;
		if (separatorChar == '\\' && length > 2 && path.charAt(1) == ':')
			firstInPath = 2;
		int index = path.lastIndexOf(separatorChar);
		if (index == -1 && firstInPath > 0)
			index = 2;
		if (index == -1 || path.charAt(length - 1) == separatorChar)
			return null;
		if (path.indexOf(separatorChar) == index
				&& path.charAt(firstInPath) == separatorChar)
			return path.substring(0, index + 1);
		return path.substring(0, index);
	}

	/**
	 * Answers a new File made from the pathname of the parent of this File.
	 * This is the path upto but not including the last name. <code>null</code>
	 * is returned when there is no parent.
	 * 
	 * @return a new File representing parent or <code>null</code>
	 */
	public File getParentFile() {
		String tempParent = getParent();
		if (tempParent == null)
			return null;
		return new File(tempParent);
	}

	/**
	 * Answers the file path of this File.
	 * 
	 * @return the file path
	 */
	public String getPath() {
		return path;
	}

	/**
	 * Answers an integer hash code for the receiver. Any two objects which
	 * answer <code>true</code> when passed to <code>equals</code> must
	 * answer the same value for this method.
	 * 
	 * @return the receiver's hash
	 * 
	 * @see #equals
	 */
	public int hashCode() {
		if (caseSensitive)
			return path.hashCode() ^ 1234321;
		else
			return path.toLowerCase().hashCode() ^ 1234321;
	}

	/**
	 * Answers if this File is an absolute pathname. Whether a pathname is
	 * absolute is platform specific. On UNIX it is if the path starts with the
	 * character '/', on Windows it is absolute if either it starts with '\',
	 * '/', '\\' (to represent a file server), or a letter followed by a colon.
	 * 
	 * @return <code>true</code> if this File is absolute, <code>false</code>
	 *         otherwise.
	 * 
	 * @see #getPath
	 */
	public boolean isAbsolute() {
		return isAbsoluteImpl(com.ibm.oti.util.Util.getBytes(path));
	}

	private native boolean isAbsoluteImpl(byte[] filePath);

	/**
	 * Answers if this File represents a <em>directory</em> on the underlying
	 * file system.
	 * 
	 * @return <code>true</code> if this File is a directory,
	 *         <code>false</code> otherwise.
	 * 
	 * @see #getPath
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 */
	public boolean isDirectory() {
		if (path.length() == 0)
			return false;
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkRead(path);
		return isDirectoryImpl(properPath(true));
	}

	private native boolean isDirectoryImpl(byte[] filePath);

	/**
	 * Answers if this File represents a <em>file</em> on the underlying file
	 * system.
	 * 
	 * @return <code>true</code> if this File is a file, <code>false</code>
	 *         otherwise.
	 * 
	 * @see #getPath
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 */
	public boolean isFile() {
		if (path.length() == 0)
			return false;
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkRead(path);
		return isFileImpl(properPath(true));
	}

	private native boolean isFileImpl(byte[] filePath);

	/**
	 * Returns wheter or not this file is a hidden file as defined by the
	 * operating system.
	 * 
	 * @return <code>true</code> if the file is hidden, <code>false</code>
	 *         otherwise.
	 */
	public boolean isHidden() {
		if (path.length() == 0)
			return false;
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkRead(path);
		return isHiddenImpl(properPath(true));
	}

	private native boolean isHiddenImpl(byte[] filePath);

	private native boolean isReadOnlyImpl(byte[] filePath);

	private native boolean isWriteOnlyImpl(byte[] filePath);

	private native byte[] getLinkImpl(byte[] filePath);

	/**
	 * Answers the time this File was last modified.
	 * 
	 * @return the time this File was last modified.
	 * 
	 * @see #getPath
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 */
	public long lastModified() {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkRead(path);
		long result = lastModifiedImpl(properPath(true));
		/* Temporary code to handle both return cases until natives fixed */
		if (result == -1 || result == 0)
			return 0;
		return (result * 1000);
	}

	private native long lastModifiedImpl(byte[] filePath);

	/**
	 * Sets the time this File was last modified.
	 * 
	 * @param time
	 *            The time to set the file as last modified.
	 * @return the time this File was last modified.
	 * 
	 * @see java.lang.SecurityManager#checkWrite(FileDescriptor)
	 */
	public boolean setLastModified(long time) {
		if (time >= 0) {
			SecurityManager security = System.getSecurityManager();
			if (security != null)
				security.checkWrite(path);
			return (setLastModifiedImpl(properPath(true), time));
		}
		throw new IllegalArgumentException(com.ibm.oti.util.Msg
				.getString("K006a")); //$NON-NLS-1$
	}

	private native boolean setLastModifiedImpl(byte[] path, long time);

	/**
	 * Marks this file or directory to be read-only as defined by the operating
	 * system.
	 * 
	 * @return <code>true</code> if the operation was a success,
	 *         <code>false</code> otherwise
	 */
	public boolean setReadOnly() {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkWrite(path);
		return (setReadOnlyImpl(properPath(true)));
	}

	private native boolean setReadOnlyImpl(byte[] path);

	/**
	 * Answers the length of this File in bytes.
	 * 
	 * @return the number of bytes in the file.
	 * 
	 * @see #getPath
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 */
	public long length() {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkRead(path);
		return lengthImpl(properPath(true));
	}

	private native long lengthImpl(byte[] filePath);

	/**
	 * Answers an array of Strings representing the file names in the directory
	 * represented by this File. If this File is not a directory the result is
	 * <code>null</code>.
	 * <p>
	 * The entries <code>.</code> and <code>..</code> representing current
	 * directory and parent directory are not returned as part of the list.
	 * 
	 * @return an array of Strings or <code>null</code>.
	 * 
	 * @see #getPath
	 * @see #isDirectory
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 */
	public java.lang.String[] list() {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkRead(path);
		if (!isDirectory())
			return null;
		byte[][] implList = listImpl(properPath(true));
		if (implList == null)
			return new String[0];
		String result[] = new String[implList.length];
		for (int index = 0; index < implList.length; index++)
			result[index] = com.ibm.oti.util.Util.toString(implList[index]);
		return result;
	}

	/**
	 * Answers an array of Files representing the file names in the directory
	 * represented by this File. If this File is not a directory the result is
	 * <code>null</code>. The Files returned will be absolute if this File is
	 * absolute, relative otherwise.
	 * 
	 * @return an array of Files or <code>null</code>.
	 * 
	 * @see #getPath
	 * @see #list()
	 * @see #isDirectory
	 */
	public File[] listFiles() {
		String[] tempNames = list();
		if (tempNames == null)
			return null;
		int resultLength = tempNames.length;
		File results[] = new File[resultLength];
		for (int i = 0; i < resultLength; i++)
			results[i] = new File(this, tempNames[i]);
		return results;
	}

	/**
	 * Answers an array of Files representing the file names in the directory
	 * represented by this File that match a specific filter. If this File is
	 * not a directory the result is <code>null</code>. If the filter is
	 * <code>null</code> then all filenames match.
	 * <p>
	 * The entries <code>.</code> and <code>..</code> representing current
	 * directory and parent directory are not returned as part of the list.
	 * 
	 * @param filter
	 *            the filter to match names to or <code>null</code>.
	 * @return an array of Files or <code>null</code>.
	 * 
	 * @see #list(FilenameFilter filter)
	 * @see #getPath
	 * @see #isDirectory
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 */
	public File[] listFiles(FilenameFilter filter) {
		String[] tempNames = list(filter);
		if (tempNames == null)
			return null;
		int resultLength = tempNames.length;
		File results[] = new File[resultLength];
		for (int i = 0; i < resultLength; i++)
			results[i] = new File(this, tempNames[i]);
		return results;
	}

	/**
	 * Answers an array of Files representing the file names in the directory
	 * represented by this File that match a specific filter. If this File is
	 * not a directory the result is <code>null</code>. If the filter is
	 * <code>null</code> then all filenames match.
	 * <p>
	 * The entries <code>.</code> and <code>..</code> representing current
	 * directory and parent directory are not returned as part of the list.
	 * 
	 * @param filter
	 *            the filter to match names to or <code>null</code>.
	 * @return an array of Files or <code>null</code>.
	 * 
	 * @see #getPath
	 * @see #isDirectory
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 */
	public File[] listFiles(FileFilter filter) {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkRead(path);
		if (!isDirectory())
			return null;
		byte[][] implList = listImpl(properPath(true));
		if (implList == null)
			return new File[0];
		java.util.Vector tempResult = new java.util.Vector();
		for (int index = 0; index < implList.length; index++) {
			String aName = com.ibm.oti.util.Util.toString(implList[index]);
			File aFile = new File(this, aName);
			if (filter == null || filter.accept(aFile))
				tempResult.addElement(aFile);
		}
		File[] result = new File[tempResult.size()];
		tempResult.copyInto(result);
		return result;
	}

	/**
	 * Answers an array of Strings representing the file names in the directory
	 * represented by this File that match a specific filter. If this File is
	 * not a directory the result is <code>null</code>. If the filter is
	 * <code>null</code> then all filenames match.
	 * <p>
	 * The entries <code>.</code> and <code>..</code> representing current
	 * directory and parent directory are not returned as part of the list.
	 * 
	 * @param filter
	 *            the filter to match names to or <code>null</code>.
	 * @return an array of Strings or <code>null</code>.
	 * 
	 * @see #getPath
	 * @see #isDirectory
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 */
	public java.lang.String[] list(FilenameFilter filter) {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkRead(path);
		if (!isDirectory())
			return null;
		byte[][] implList = listImpl(properPath(true));
		if (implList == null)
			return new String[0];
		java.util.Vector tempResult = new java.util.Vector();
		for (int index = 0; index < implList.length; index++) {
			String aName = com.ibm.oti.util.Util.toString(implList[index]);
			if (filter == null || filter.accept(this, aName))
				tempResult.addElement(aName);
		}
		String[] result = new String[tempResult.size()];
		tempResult.copyInto(result);
		return result;
	}

	private synchronized static native byte[][] listImpl(byte[] path);

	/**
	 * Creates the directory named by the trailing filename of this File. Not
	 * all directories required to create this File are created.
	 * 
	 * @return <code>true</code> if the directory was created,
	 *         <code>false</code> otherwise.
	 * 
	 * @see #getPath
	 * @see java.lang.SecurityManager#checkWrite(FileDescriptor)
	 */
	public boolean mkdir() {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkWrite(path);
		return mkdirImpl(properPath(true));
	}

	private native boolean mkdirImpl(byte[] filePath);

	/**
	 * Create all the directories needed for this File. If the terminal
	 * directory already exists, answer false. If the directories were created
	 * successfully, answer <code>true</code>.
	 * 
	 * @return <code>true</code> if the neccessary directories were created,
	 *         <code>false</code> otherwise.
	 * 
	 */
	public boolean mkdirs() {
		/* If the terminal directory already exists, answer false */
		if (exists())
			return false;

		/* If the receiver can be created, answer true */
		if (mkdir())
			return true;

		String parentDir = getParent();
		/* If there is no parent and we were not created, answer false */
		if (parentDir == null)
			return false;

		/* Otherwise, try to create a parent directory and then this directory */
		return (new File(parentDir).mkdirs() && mkdir());
	}

	/**
	 * Creates the file specified by this File. If the file already exists this
	 * method returns <code>false</code>. Otherwise, if the file is created
	 * successfully, the result is <code>true</code>. An IOException will be
	 * thrown if the directory to contain this file does not exist.
	 * 
	 * @return <code>true</code> if this File was created, <code>false</code>
	 *         otherwise.
	 * 
	 * @throws IOException
	 *             if an I/O error occurs or the directory does not exist.
	 * 
	 * @see java.lang.SecurityManager#checkWrite(FileDescriptor)
	 */
	public boolean createNewFile() throws IOException {
		SecurityManager security = System.getSecurityManager();
		if (security != null)
			security.checkWrite(path);
		int result = newFileImpl(properPath(true));
		switch (result) {
		case 0:
			return true;
		case 1:
			return false;
		case 3:
			throw new IOException(com.ibm.oti.util.Msg.getString("K01c1")); //$NON-NLS-1$
		default:
			throw new IOException(com.ibm.oti.util.Msg.getString("K01c2", path)); //$NON-NLS-1$
		}
	}

	private native int newFileImpl(byte[] filePath);

	/**
	 * Creates an empty temporary file using the given prefix and suffix as part
	 * of the file name. If suffix is null, <code>.tmp</code> is used.
	 * 
	 * @param prefix
	 *            the prefix to the temp file name
	 * @param suffix
	 *            the suffix to the temp file name
	 * @return the temporary file
	 * 
	 * @throws IOException
	 *             If an error occurs when writing the file
	 */
	public static File createTempFile(String prefix, String suffix)
			throws IOException {
		return createTempFile(prefix, suffix, null);
	}

	/**
	 * Creates an empty temporary file in the given directory using the given
	 * prefix and suffix as part of the file name.
	 * 
	 * @param prefix
	 *            the prefix to the temp file name
	 * @param suffix
	 *            the suffix to the temp file name
	 * @param directory
	 *            the location to which the temp file is to be written, or null
	 *            for the default temp location
	 * @return the temporary file
	 * 
	 * @throws IOException
	 *             If an error occurs when writing the file
	 */
	public static File createTempFile(String prefix, String suffix,
			File directory) throws IOException {
		if (prefix != null) {
			if (prefix.length() >= 3) {
				String newSuffix = suffix == null ? ".tmp" : suffix; //$NON-NLS-1$
				String tmpDir = "."; //$NON-NLS-1$
				tmpDir = (String) AccessController
						.doPrivileged(new PriviAction("java.io.tmpdir", "."));   //$NON-NLS-1$//$NON-NLS-2$
				File result, tmpDirFile = directory == null ? new File(tmpDir)
						: directory;
				do {
					result = genTempFile(prefix, newSuffix, tmpDirFile);
				} while (!result.createNewFile());
				return result;
			}
			throw new IllegalArgumentException(com.ibm.oti.util.Msg
					.getString("K006b")); //$NON-NLS-1$
		}
		throw new NullPointerException();
	}

	private static File genTempFile(String prefix, String suffix, File directory) {
		if (counter == 0) {
			int newInt = new java.util.Random().nextInt();
			counter = ((newInt / 65535) & 0xFFFF) + 0x2710;
		}
		StringBuffer newName = new StringBuffer();
		newName.append(prefix);
		newName.append(counter++);
		newName.append(suffix);
		return new File(directory, newName.toString());
	}

	/**
	 * Answer a String representing the proper path for the receiver. If the
	 * receiver is absolute do not prepend the user.dir property, otherwise do.
	 * 
	 * @param internal
	 *            is user.dir internal
	 * @return the proper path
	 */
	byte[] properPath(boolean internal) {
		if (properPath != null)
			return properPath;
		byte[] pathBytes = com.ibm.oti.util.Util.getBytes(path);
		if (isAbsoluteImpl(pathBytes))
			return properPath = pathBytes;
		// Check security by getting user.dir when the path is not absolute
		String userdir;
		if (internal) {
			userdir = (String) AccessController.doPrivileged(new PriviAction(
					"user.dir")); //$NON-NLS-1$
		} else
			userdir = System.getProperty("user.dir"); //$NON-NLS-1$
		if ((properPath = properPathImpl(pathBytes)) != null)
			return properPath;
		if (path.length() == 0)
			return properPath = com.ibm.oti.util.Util.getBytes(userdir);
		int length = userdir.length();
		if (path.charAt(0) == '\\') {
			if (length > 1 && userdir.charAt(1) == ':') {
				return properPath = com.ibm.oti.util.Util.getBytes(userdir
						.substring(0, 2)
						+ path);
			}
			if (length > 0 && userdir.charAt(length - 1) == separatorChar) {
				return properPath = com.ibm.oti.util.Util.getBytes(userdir
						+ path.substring(1));
			}
			return properPath = com.ibm.oti.util.Util.getBytes(userdir + path);
		}
		if (length > 0 && userdir.charAt(length - 1) == separatorChar) {
			return properPath = com.ibm.oti.util.Util.getBytes(userdir + path);
		}
		return properPath = com.ibm.oti.util.Util.getBytes(userdir + separator
				+ path);
	}

	private static native byte[] properPathImpl(byte[] path);

	/**
	 * Renames this File to the name represented by the File <code>dest</code>.
	 * This works for both normal files and directories.
	 * 
	 * @param dest
	 *            the File containing the new name.
	 * @return <code>true</code> if the File was renamed, <code>false</code>
	 *         otherwise.
	 * 
	 * @see #getPath
	 * @see java.lang.SecurityManager#checkRead(FileDescriptor)
	 * @see java.lang.SecurityManager#checkWrite(FileDescriptor)
	 */
	public boolean renameTo(java.io.File dest) {
		SecurityManager security = System.getSecurityManager();
		if (security != null) {
			security.checkWrite(path);
			security.checkWrite(dest.path);
		}
		return renameToImpl(properPath(true), dest.properPath(true));
	}

	private native boolean renameToImpl(byte[] pathExist, byte[] pathNew);

	/**
	 * Answers a string containing a concise, human-readable description of the
	 * receiver.
	 * 
	 * @return a printable representation for the receiver.
	 */
	public String toString() {
		return path.toString();
	}

	/**
	 * Answers a <code>file</code> URI for this File. The URI is System
	 * dependant and may not be transferrable between different operating/file
	 * systems.
	 * 
	 * @return a <code>file</code> URI for this File.
	 */
	public URI toURI() {
		String name = getAbsoluteName();
		try {
			if (!name.startsWith("/")) // On Windows, absolute paths might not //$NON-NLS-1$
										// start with sep.
				return new URI("file", null, //$NON-NLS-1$
						new StringBuffer(name.length() + 1).append('/').append(
								name).toString(), null, null);
			else if (name.startsWith("//")) //$NON-NLS-1$
				return new URI("file", name, null); // UNC path //$NON-NLS-1$
			return new URI("file", null, name, null, null); //$NON-NLS-1$
		} catch (URISyntaxException e) {
			// this should never happen
			return null;
		}
	}

	/**
	 * Answers a <code>file</code> URL for this File. The URL is System
	 * dependant and may not be transferrable between different operating/file
	 * systems.
	 * 
	 * @return a <code>file</code> URL for this File.
	 * 
	 * @throws java.net.MalformedURLException
	 *             if the path cannot be transformed into an URL
	 */
	public URL toURL() throws java.net.MalformedURLException {
		String name = getAbsoluteName();
		if (!name.startsWith("/")) // On Windows, absolute paths might not //$NON-NLS-1$
									// start with sep.
			return new URL("file", "", -1, new StringBuffer(name.length() + 1) //$NON-NLS-1$ //$NON-NLS-2$
					.append('/').append(name).toString(), null);
		else if (name.startsWith("//")) //$NON-NLS-1$
			return new URL("file:" + name); // UNC path //$NON-NLS-1$
		return new URL("file", "", -1, name, null); //$NON-NLS-1$ //$NON-NLS-2$
	}

	private String getAbsoluteName() {
		String name = getAbsolutePath();
		if (isDirectory() && name.charAt(name.length() - 1) != separatorChar) // Directories
																				// must
																				// end
																				// with
																				// a
																				// slash
			name = new StringBuffer(name.length() + 1).append(name).append('/')
					.toString();
		if (separatorChar != '/') // Must convert slashes.
			name = name.replace(separatorChar, '/');
		return name;
	}

	private void writeObject(ObjectOutputStream stream) throws IOException {
		stream.defaultWriteObject();
		stream.writeChar(separatorChar);

	}

	private void readObject(ObjectInputStream stream) throws IOException,
			ClassNotFoundException {
		stream.defaultReadObject();
		char inSeparator = stream.readChar();
		path = path.replace(inSeparator, separatorChar);
	}

}
