/*
 *  Copyright 2005 The Apache Software Foundation or its licensors, as applicable.
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
 * @author Hugo Beilis
 * @author Osvaldo Demo
 * @author Jorge Rafael
 * @version 1.0
 */

package ar.org.fitc.test.integration.crypto.util;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.security.Key;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.interfaces.RSAPrivateKey;
import java.security.interfaces.RSAPublicKey;

public class K implements Key {

    private static KeyPair kp = null;

    private static byte[] keyPairEncode = { -84, -19, 0, 5, 115, 114, 0, 21,
            106, 97, 118, 97, 46, 115, 101, 99, 117, 114, 105, 116, 121, 46,
            75, 101, 121, 80, 97, 105, 114, -105, 3, 12, 58, -46, -51, 18,
            -109, 2, 0, 2, 76, 0, 10, 112, 114, 105, 118, 97, 116, 101, 75,
            101, 121, 116, 0, 26, 76, 106, 97, 118, 97, 47, 115, 101, 99, 117,
            114, 105, 116, 121, 47, 80, 114, 105, 118, 97, 116, 101, 75, 101,
            121, 59, 76, 0, 9, 112, 117, 98, 108, 105, 99, 75, 101, 121, 116,
            0, 25, 76, 106, 97, 118, 97, 47, 115, 101, 99, 117, 114, 105, 116,
            121, 47, 80, 117, 98, 108, 105, 99, 75, 101, 121, 59, 120, 112,
            115, 114, 0, 49, 111, 114, 103, 46, 98, 111, 117, 110, 99, 121, 99,
            97, 115, 116, 108, 101, 46, 106, 99, 101, 46, 112, 114, 111, 118,
            105, 100, 101, 114, 46, 74, 67, 69, 82, 83, 65, 80, 114, 105, 118,
            97, 116, 101, 67, 114, 116, 75, 101, 121, 108, -70, -121, -50, 2,
            115, 85, 46, 2, 0, 6, 76, 0, 14, 99, 114, 116, 67, 111, 101, 102,
            102, 105, 99, 105, 101, 110, 116, 116, 0, 22, 76, 106, 97, 118, 97,
            47, 109, 97, 116, 104, 47, 66, 105, 103, 73, 110, 116, 101, 103,
            101, 114, 59, 76, 0, 14, 112, 114, 105, 109, 101, 69, 120, 112,
            111, 110, 101, 110, 116, 80, 113, 0, 126, 0, 5, 76, 0, 14, 112,
            114, 105, 109, 101, 69, 120, 112, 111, 110, 101, 110, 116, 81, 113,
            0, 126, 0, 5, 76, 0, 6, 112, 114, 105, 109, 101, 80, 113, 0, 126,
            0, 5, 76, 0, 6, 112, 114, 105, 109, 101, 81, 113, 0, 126, 0, 5, 76,
            0, 14, 112, 117, 98, 108, 105, 99, 69, 120, 112, 111, 110, 101,
            110, 116, 113, 0, 126, 0, 5, 120, 114, 0, 46, 111, 114, 103, 46,
            98, 111, 117, 110, 99, 121, 99, 97, 115, 116, 108, 101, 46, 106,
            99, 101, 46, 112, 114, 111, 118, 105, 100, 101, 114, 46, 74, 67,
            69, 82, 83, 65, 80, 114, 105, 118, 97, 116, 101, 75, 101, 121, -78,
            53, -117, 64, 29, 49, -123, 86, 3, 0, 4, 76, 0, 7, 109, 111, 100,
            117, 108, 117, 115, 113, 0, 126, 0, 5, 76, 0, 16, 112, 107, 99,
            115, 49, 50, 65, 116, 116, 114, 105, 98, 117, 116, 101, 115, 116,
            0, 21, 76, 106, 97, 118, 97, 47, 117, 116, 105, 108, 47, 72, 97,
            115, 104, 116, 97, 98, 108, 101, 59, 76, 0, 14, 112, 107, 99, 115,
            49, 50, 79, 114, 100, 101, 114, 105, 110, 103, 116, 0, 18, 76, 106,
            97, 118, 97, 47, 117, 116, 105, 108, 47, 86, 101, 99, 116, 111,
            114, 59, 76, 0, 15, 112, 114, 105, 118, 97, 116, 101, 69, 120, 112,
            111, 110, 101, 110, 116, 113, 0, 126, 0, 5, 120, 112, 115, 114, 0,
            20, 106, 97, 118, 97, 46, 109, 97, 116, 104, 46, 66, 105, 103, 73,
            110, 116, 101, 103, 101, 114, -116, -4, -97, 31, -87, 59, -5, 29,
            3, 0, 6, 73, 0, 8, 98, 105, 116, 67, 111, 117, 110, 116, 73, 0, 9,
            98, 105, 116, 76, 101, 110, 103, 116, 104, 73, 0, 19, 102, 105,
            114, 115, 116, 78, 111, 110, 122, 101, 114, 111, 66, 121, 116, 101,
            78, 117, 109, 73, 0, 12, 108, 111, 119, 101, 115, 116, 83, 101,
            116, 66, 105, 116, 73, 0, 6, 115, 105, 103, 110, 117, 109, 91, 0,
            9, 109, 97, 103, 110, 105, 116, 117, 100, 101, 116, 0, 2, 91, 66,
            120, 114, 0, 16, 106, 97, 118, 97, 46, 108, 97, 110, 103, 46, 78,
            117, 109, 98, 101, 114, -122, -84, -107, 29, 11, -108, -32, -117,
            2, 0, 0, 120, 112, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2,
            -1, -1, -1, -2, 0, 0, 0, 1, 117, 114, 0, 2, 91, 66, -84, -13, 23,
            -8, 6, 8, 84, -32, 2, 0, 0, 120, 112, 0, 0, 0, 64, -101, -128, 59,
            -121, 111, -5, 96, -60, -17, 39, 88, 64, -35, -1, 24, 90, -3, -42,
            -62, -105, 124, -37, 44, 79, 31, -108, 62, -11, 117, 105, 60, 25,
            -119, -89, 99, 52, -78, -116, -9, -12, -1, -2, -38, -17, 37, 97,
            -57, 112, 33, -68, -22, -28, 15, 26, -34, 96, -55, -102, -74, -82,
            35, -2, 2, 27, 120, 115, 114, 0, 19, 106, 97, 118, 97, 46, 117,
            116, 105, 108, 46, 72, 97, 115, 104, 116, 97, 98, 108, 101, 19,
            -69, 15, 37, 33, 74, -28, -72, 3, 0, 2, 70, 0, 10, 108, 111, 97,
            100, 70, 97, 99, 116, 111, 114, 73, 0, 9, 116, 104, 114, 101, 115,
            104, 111, 108, 100, 120, 112, 63, 64, 0, 0, 0, 0, 0, 8, 119, 8, 0,
            0, 0, 11, 0, 0, 0, 0, 120, 115, 114, 0, 16, 106, 97, 118, 97, 46,
            117, 116, 105, 108, 46, 86, 101, 99, 116, 111, 114, -39, -105, 125,
            91, -128, 59, -81, 1, 3, 0, 3, 73, 0, 17, 99, 97, 112, 97, 99, 105,
            116, 121, 73, 110, 99, 114, 101, 109, 101, 110, 116, 73, 0, 12,
            101, 108, 101, 109, 101, 110, 116, 67, 111, 117, 110, 116, 91, 0,
            11, 101, 108, 101, 109, 101, 110, 116, 68, 97, 116, 97, 116, 0, 19,
            91, 76, 106, 97, 118, 97, 47, 108, 97, 110, 103, 47, 79, 98, 106,
            101, 99, 116, 59, 120, 112, 0, 0, 0, 0, 0, 0, 0, 0, 117, 114, 0,
            19, 91, 76, 106, 97, 118, 97, 46, 108, 97, 110, 103, 46, 79, 98,
            106, 101, 99, 116, 59, -112, -50, 88, -97, 16, 115, 41, 108, 2, 0,
            0, 120, 112, 0, 0, 0, 10, 112, 112, 112, 112, 112, 112, 112, 112,
            112, 112, 120, 115, 113, 0, 126, 0, 10, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -2, -1, -1, -1, -2, 0, 0, 0, 1, 117, 113, 0, 126,
            0, 14, 0, 0, 0, 64, 30, 46, -16, -33, 36, 52, 102, 31, 51, -48,
            -82, 71, -63, 15, -113, -7, -79, -123, -21, 53, -98, 109, -88,
            -105, 31, 41, 22, 99, 108, -53, -27, 119, 61, 108, 15, -87, 28,
            -14, 1, -98, 20, -109, -70, -68, -82, 76, -110, -114, -30, -117,
            22, -101, -77, -97, 23, -62, -128, 20, -106, -40, 117, -22, -64,
            -79, 120, 120, 115, 113, 0, 126, 0, 10, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -2, -1, -1, -1, -2, 0, 0, 0, 1, 117, 113, 0, 126,
            0, 14, 0, 0, 0, 32, 50, -42, -104, -15, -63, -76, -93, -83, -109,
            109, 9, -19, 88, -47, -116, 71, -23, -104, 27, 72, -34, 89, -3,
            -102, 29, -12, -87, -75, 72, 112, -96, -20, 120, 115, 113, 0, 126,
            0, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -1, -1, -1,
            -2, 0, 0, 0, 1, 117, 113, 0, 126, 0, 14, 0, 0, 0, 32, -122, -34,
            23, -121, 8, -70, -25, -8, -73, -91, 127, 103, 45, 64, -104, -2,
            126, -3, 98, 68, 97, -40, -5, -61, -35, 83, -3, 52, 43, -23, 88,
            19, 120, 115, 113, 0, 126, 0, 10, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -2, -1, -1, -1, -2, 0, 0, 0, 1, 117, 113, 0, 126, 0,
            14, 0, 0, 0, 32, 52, -35, 93, 61, -126, 105, -102, -81, -126, 79,
            -96, -57, -63, -85, 86, 82, 3, -15, -60, 2, 101, 93, 7, -26, 83,
            -13, -108, 84, 9, 52, -58, -39, 120, 115, 113, 0, 126, 0, 10, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -1, -1, -1, -2, 0, 0,
            0, 1, 117, 113, 0, 126, 0, 14, 0, 0, 0, 32, -29, 84, 117, 66, 59,
            106, 85, 47, 14, 1, -120, -47, 63, -3, -125, 70, -72, -128, 95,
            -86, -78, 42, 1, 45, 117, 55, -88, -31, 7, 89, -14, 83, 120, 115,
            113, 0, 126, 0, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2,
            -1, -1, -1, -2, 0, 0, 0, 1, 117, 113, 0, 126, 0, 14, 0, 0, 0, 32,
            -81, 28, -74, -108, -7, -21, -117, 104, 67, 96, 42, 31, 41, -60,
            -61, -62, -104, 47, -46, -126, 27, -128, 72, 22, -99, -104, -45,
            -56, 57, -6, 72, 25, 120, 115, 113, 0, 126, 0, 10, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -2, -1, -1, -1, -2, 0, 0, 0, 1, 117,
            113, 0, 126, 0, 14, 0, 0, 0, 3, 1, 0, 1, 120, 115, 114, 0, 45, 111,
            114, 103, 46, 98, 111, 117, 110, 99, 121, 99, 97, 115, 116, 108,
            101, 46, 106, 99, 101, 46, 112, 114, 111, 118, 105, 100, 101, 114,
            46, 74, 67, 69, 82, 83, 65, 80, 117, 98, 108, 105, 99, 75, 101,
            121, 37, 34, 106, 14, 91, -6, 108, -124, 2, 0, 2, 76, 0, 7, 109,
            111, 100, 117, 108, 117, 115, 113, 0, 126, 0, 5, 76, 0, 14, 112,
            117, 98, 108, 105, 99, 69, 120, 112, 111, 110, 101, 110, 116, 113,
            0, 126, 0, 5, 120, 112, 113, 0, 126, 0, 13, 113, 0, 126, 0, 35 };

    static final long serialVersionUID = 2L;

    private final byte[] keyBytes;

    private final String alg;

    private final String format;

    private static void keyPairDecodec() {
        try {
            ObjectInputStream in = new ObjectInputStream(
                    new ByteArrayInputStream(keyPairEncode));
            kp = (KeyPair) in.readObject();
        } catch (Exception e) {
        }
        ;
    }

    public static RSAPublicKey getRSAPublicKey() {
        if (kp == null) {
            keyPairDecodec();
        }
        return (RSAPublicKey) kp.getPublic();
    }

    public static RSAPrivateKey getRSAPrivateKey() {
        if (kp == null) {
            keyPairDecodec();
        }
        return (RSAPrivateKey) kp.getPrivate();
    }

    public static void KeyPairGenerate() {
        try {
            KeyPairGenerator kpg = KeyPairGenerator.getInstance("RSA", "BC");
            kpg.initialize(1024);
            kp = kpg.genKeyPair();
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            ObjectOutputStream oout = new ObjectOutputStream(out);
            oout.writeObject(kp);
            keyPairEncode = out.toByteArray();
        } catch (Exception e) {
        }
    }



    public K(String alg, byte[] keyBytes) {
        this.alg = alg;
        this.keyBytes = keyBytes;
        this.format = "RAW";
    }

    public String getAlgorithm() {
        return alg;
    }

    public String getFormat() {
        return format;
    }

    public byte[] getEncoded() {
        return (byte[]) keyBytes.clone();
    }

}
