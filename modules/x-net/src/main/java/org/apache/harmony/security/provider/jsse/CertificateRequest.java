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

/**
 * @author Boris Kuznetsov
 * @version $Revision$
 */

package org.apache.harmony.security.provider.jsse;

import java.io.IOException;
import java.security.cert.X509Certificate;

import javax.security.auth.x500.X500Principal;

/**
 * 
 * Represents certificate request message
 * @see TLS 1.0 spec., 7.4.4. Certificate request
 * (http://www.ietf.org/rfc/rfc2246.txt)
 */
public class CertificateRequest extends Message {

    /**
     * Client certificate types as defined in 
     * TLS 1.0 spec., 7.4.4. Certificate request
     */
    public static final byte RSA_SIGN = 1;
    public static final byte DSS_SIGN = 2;
    public static final byte RSA_FIXED_DH = 3;
    public static final byte DSS_FIXED_DH = 4;

    /**
     * Requested certificate types
     */
    final byte[] certificate_types;

    /**
     * Certificate authorities
     */
    final X500Principal[] certificate_authorities;

    //Requested certificate types as Strings
    // ("RSA", "DSA", "DH_RSA" or "DH_DSA")
    private String[] types;

    // Encoded form of certificate authorities
    private byte[][] encoded_principals;

    /**
     * Creates outbound message
     * 
     * @param certificate_types
     * @param accepted - array of certificate authority certificates
     */
    public CertificateRequest(byte[] certificate_types,
            X509Certificate[] accepted) {

        if (accepted == null) {
            fatalAlert(AlertProtocol.INTERNAL_ERROR,
                    "CertificateRequest: array of certificate authority certificates is null");
        }
        this.certificate_types = certificate_types;
        
        int totalPrincipalsLength = 0;
        certificate_authorities = new X500Principal[accepted.length];
        encoded_principals = new byte[accepted.length][];
        for (int i = 0; i < accepted.length; i++) {
            certificate_authorities[i] = accepted[i].getIssuerX500Principal();
            encoded_principals[i] = certificate_authorities[i].getEncoded();
            totalPrincipalsLength += encoded_principals[i].length + 2;
        }

        length = 3 + certificate_types.length + totalPrincipalsLength;
    }

    /**
     * Creates inbound message
     * 
     * @param in
     * @param length
     * @throws IOException
     */
    public CertificateRequest(HandshakeIODataStream in, int length)
            throws IOException {
        int size = in.readUint8();
        certificate_types = new byte[size];
        in.read(certificate_types, 0, size);
        size = in.readUint16();
        certificate_authorities = new X500Principal[size];
        int totalPrincipalsLength = 0;
        int principalLength = 0;
        for (int i = 0; i < size; i++) {
            principalLength = in.readUint16(); // encoded X500Principal size
            certificate_authorities[i] = new X500Principal(in);
            totalPrincipalsLength += 2;
            totalPrincipalsLength += principalLength;
        }
        this.length = 3 + certificate_types.length + totalPrincipalsLength;
        if (this.length != length) {
            fatalAlert(AlertProtocol.DECODE_ERROR,
                    "DECODE ERROR: incorrect CertificateRequest");
        }

    }

    /**
     * Sends message
     * 
     * @param out
     */
    public void send(HandshakeIODataStream out) {

        out.writeUint8(certificate_types.length);
        for (int i = 0; i < certificate_types.length; i++) {
            out.write(certificate_types[i]);
        }
        out.writeUint16(certificate_authorities.length);
        for (int i = 0; i < certificate_authorities.length; i++) {
            out.writeUint16(encoded_principals[i].length);
            out.write(encoded_principals[i]);
        }
    }

    /**
     * Returns message type
     * 
     * @return
     */
    public int getType() {
        return Handshake.CERTIFICATE_REQUEST;
    }

    /**
     * Returns requested certificate types as array of strings
     */
    public String[] getTypesAsString() {
        if (types == null) {
            types = new String[certificate_types.length];
            for (int i = 0; i < types.length; i++) {
                switch (certificate_types[i]) {
                case 1:
                    types[i] = "RSA";
                    break;
                case 2:
                    types[i] = "DSA";
                    break;
                case 3:
                    types[i] = "DH_RSA";
                    break;
                case 4:
                    types[i] = "DH_DSA";
                    break;
                default:
                    fatalAlert(AlertProtocol.DECODE_ERROR,
                            "DECODE ERROR: incorrect CertificateRequest");
                }
            }
        }
        return types;
    }

}