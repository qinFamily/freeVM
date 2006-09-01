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

package org.apache.harmony.sound.tests.javax.sound.midi.spi;

import javax.sound.midi.MidiDevice;
import javax.sound.midi.spi.MidiDeviceProvider;

import junit.framework.TestCase;

public class MidiDeviceProviderTest extends TestCase {

    static MidiDevice.Info[] recorded;

    /**
     * @tests javax.sound.midi.spi.MidiDeviceProvider#isDeviceSupported(MidiDevice.Info)
     */
    public void testIsDeviceSupported() {
        MidiDeviceProvider myProvider = new MidiDeviceProvider() {

            public MidiDevice getDevice(MidiDevice.Info info) {
                fail("what are doing here?");
                return null;
            }

            public MidiDevice.Info[] getDeviceInfo() {
                return recorded;
            }
        };

        recorded = new MidiDevice.Info[] { new MidiDevice.Info("1", "2", "3",
                "4") {} };
        assertTrue(myProvider.isDeviceSupported(recorded[0]));

        MidiDevice.Info similar = new MidiDevice.Info("1", "2", "3", "4") {};
        assertFalse(myProvider.isDeviceSupported(similar));

        try {
            myProvider.isDeviceSupported(null);
            fail("NPE expected");
        } catch (NullPointerException e) {}

        recorded = null;
        try {
            myProvider.isDeviceSupported(similar);
            fail("NPE expected");
        } catch (NullPointerException e) {}
    }
}
