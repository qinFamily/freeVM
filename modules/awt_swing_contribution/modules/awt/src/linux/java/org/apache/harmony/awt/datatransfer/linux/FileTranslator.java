/*
 *  Copyright 2005 - 2006 The Apache Software Software Foundation or its licensors, as applicable.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/**
 * @author Michael Danilov
 * @version $Revision$
 */
package org.apache.harmony.awt.datatransfer.linux;

import java.io.*;
import java.util.*;
import java.awt.datatransfer.*;

import org.apache.harmony.awt.datatransfer.*;

final class FileTranslator implements ClipboardTranslator {

    public byte[] java2Native(Transferable contents, TranslationPoint point) throws Exception {
        StringBuffer buf = new StringBuffer();

        for (Iterator i = ((List) contents.getTransferData(point.flavor)).iterator(); ;) {
            buf.append(((File) i.next()).toString());
            if (i.hasNext()) {
                buf.append('\n');
            } else {
                break;
            }
        }

        return buf.toString().getBytes("iso8859-1");
    }

    public Object native2Java(byte[] contents, TranslationPoint point) throws Exception {
        LinkedList list = new LinkedList();
        String files[] = new String(contents, "iso8859-1").split("\n");

        for (int i = 0; i < files.length; i++) {
            list.add(new File(files[i]));
        }

        return list;
    }

    public void assigned2Point(TranslationPoint point) throws UnsupportedTranslationPoint {
        if (!point.flavor.equals(DataFlavor.javaFileListFlavor)
                || !point.natife.equals(LinuxFlavorMap.FILE_NAME))
        {
            throw new UnsupportedTranslationPoint(point.toString());
        }
    }

}
