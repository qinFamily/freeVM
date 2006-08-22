/*
 * RMI skeleton class
 * for class org.apache.harmony.jndi.provider.rmi.registry.RemoteReferenceWrapper
 * Compatible with stub protocol version 1.1/1.2
 *
 * Generated by DRL RMI Compiler (rmic).
 *
 * DO NOT EDIT!!!
 * Contents subject to change without notice!
 */
package org.apache.harmony.jndi.provider.rmi.registry;


public final class RemoteReferenceWrapper_Skel implements java.rmi.server.Skeleton {

    private static final long interfaceHash = 2534274963554139942L;

    private static final java.rmi.server.Operation[] operations = {
        new java.rmi.server.Operation("javax.naming.Reference getReference()")
    };

    public java.rmi.server.Operation[] getOperations() {
        return (java.rmi.server.Operation[]) operations.clone();
    }

    public void dispatch(java.rmi.Remote obj, java.rmi.server.RemoteCall call, int opnum, long hash) throws java.lang.Exception {
        if (opnum < 0) {
            if (hash == 3529874867989176284L) {
                opnum = 0;
            } else {
                throw new java.rmi.UnmarshalException("Invalid method hash: " + hash);
            }
        } else {
            if (hash != interfaceHash) {
                throw new java.rmi.server.SkeletonMismatchException(
                        "Interface hash mismatch, expected: " + interfaceHash + ", received: " + hash);
            }
        }

        org.apache.harmony.jndi.provider.rmi.registry.RemoteReferenceWrapper server = (org.apache.harmony.jndi.provider.rmi.registry.RemoteReferenceWrapper) obj;

        switch (opnum) {

        case 0: {    // getReference()

            call.releaseInputStream();

            javax.naming.Reference $result = server.getReference();

            try {
                java.io.ObjectOutput out = call.getResultStream(true);
                out.writeObject($result);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException("Error marshalling return", e);
            }

            break;
        }

        default:
            throw new java.rmi.UnmarshalException("Invalid method number: " + opnum);
        }
    }
}
