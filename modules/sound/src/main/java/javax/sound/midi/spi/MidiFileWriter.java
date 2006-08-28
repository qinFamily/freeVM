/*
 *  Copyright 2006 The Apache Software Foundation or its licensors, as applicable.
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

package javax.sound.midi.spi;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;

import javax.sound.midi.Sequence;

public abstract class MidiFileWriter {

    public abstract int[] getMidiFileTypes();

    public abstract int[] getMidiFileTypes(Sequence sequence);

    public boolean isFileTypeSupported(int fileType) {
        int[] supported = getMidiFileTypes();
        for (int i = 0; i < supported.length; i++) {
            if (fileType == supported[i]) {
                return true;
            }
        }
        return false;
    }

    public boolean isFileTypeSupported(int fileType, Sequence sequence) {
        int[] supported = getMidiFileTypes(sequence);
        for (int i = 0; i < supported.length; i++) {
            if (fileType == supported[i]) {
                return true;
            }
        }
        return false;
    }

    public abstract int write(Sequence in, int fileType, File out)
            throws IOException;

    public abstract int write(Sequence in, int fileType, OutputStream out)
            throws IOException;
}
