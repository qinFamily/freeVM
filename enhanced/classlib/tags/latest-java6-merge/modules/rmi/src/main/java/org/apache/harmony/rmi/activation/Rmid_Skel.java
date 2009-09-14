/*
 * RMI skeleton class
 * for class org.apache.harmony.rmi.activation.Rmid
 * Compatible with stub protocol version 1.1/1.2
 *
 * Generated by DRL RMI Compiler (rmic).
 *
 * DO NOT EDIT!!!
 * Contents subject to change without notice!
 */
package org.apache.harmony.rmi.activation;

import org.apache.harmony.rmi.internal.nls.Messages;


public final class Rmid_Skel implements java.rmi.server.Skeleton {

    private static final long interfaceHash = 8470858815147946311L;

    private static final java.rmi.server.Operation[] operations = {
        new java.rmi.server.Operation("java.rmi.MarshalledObject activate(java.rmi.activation.ActivationID, boolean)"), //$NON-NLS-1$
        new java.rmi.server.Operation("java.rmi.activation.ActivationMonitor activeGroup(java.rmi.activation.ActivationGroupID, java.rmi.activation.ActivationInstantiator, long)"), //$NON-NLS-1$
        new java.rmi.server.Operation("void activeObject(java.rmi.activation.ActivationID, java.rmi.MarshalledObject)"), //$NON-NLS-1$
        new java.rmi.server.Operation("java.rmi.activation.ActivationDesc getActivationDesc(java.rmi.activation.ActivationID)"), //$NON-NLS-1$
        new java.rmi.server.Operation("java.rmi.activation.ActivationGroupDesc getActivationGroupDesc(java.rmi.activation.ActivationGroupID)"), //$NON-NLS-1$
        new java.rmi.server.Operation("void inactiveGroup(java.rmi.activation.ActivationGroupID, long)"), //$NON-NLS-1$
        new java.rmi.server.Operation("void inactiveObject(java.rmi.activation.ActivationID)"), //$NON-NLS-1$
        new java.rmi.server.Operation("java.rmi.activation.ActivationGroupID registerGroup(java.rmi.activation.ActivationGroupDesc)"), //$NON-NLS-1$
        new java.rmi.server.Operation("java.rmi.activation.ActivationID registerObject(java.rmi.activation.ActivationDesc)"), //$NON-NLS-1$
        new java.rmi.server.Operation("java.rmi.activation.ActivationDesc setActivationDesc(java.rmi.activation.ActivationID, java.rmi.activation.ActivationDesc)"), //$NON-NLS-1$
        new java.rmi.server.Operation("java.rmi.activation.ActivationGroupDesc setActivationGroupDesc(java.rmi.activation.ActivationGroupID, java.rmi.activation.ActivationGroupDesc)"), //$NON-NLS-1$
        new java.rmi.server.Operation("void shutdown()"), //$NON-NLS-1$
        new java.rmi.server.Operation("void unregisterGroup(java.rmi.activation.ActivationGroupID)"), //$NON-NLS-1$
        new java.rmi.server.Operation("void unregisterObject(java.rmi.activation.ActivationID)") //$NON-NLS-1$
    };

    public java.rmi.server.Operation[] getOperations() {
        return (java.rmi.server.Operation[]) operations.clone();
    }

    public void dispatch(java.rmi.Remote obj, java.rmi.server.RemoteCall call, int opnum, long hash) throws java.lang.Exception {
        if (opnum < 0) {
            if (hash == -8767355154875805558L) {
                opnum = 0;
            } else if (hash == -4575843150759415294L) {
                opnum = 1;
            } else if (hash == 2543984342209939736L) {
                opnum = 2;
            } else if (hash == 4830055440982622087L) {
                opnum = 3;
            } else if (hash == -8701843806548736528L) {
                opnum = 4;
            } else if (hash == -399287892768650944L) {
                opnum = 5;
            } else if (hash == -4165404120701281807L) {
                opnum = 6;
            } else if (hash == 6921515268192657754L) {
                opnum = 7;
            } else if (hash == -3006759798994351347L) {
                opnum = 8;
            } else if (hash == 7128043237057180796L) {
                opnum = 9;
            } else if (hash == 1213918527826541191L) {
                opnum = 10;
            } else if (hash == -7207851917985848402L) {
                opnum = 11;
            } else if (hash == 3768097077835970701L) {
                opnum = 12;
            } else if (hash == -6843850585331411084L) {
                opnum = 13;
            } else {
                // rmi.28=Invalid method hash: {0}
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.28", hash)); //$NON-NLS-1$
            }
        } else {
            if (hash != interfaceHash) {
                // rmi.29=Interface hash mismatch, expected: {0}, received: {1}
                throw new java.rmi.server.SkeletonMismatchException(
                        Messages.getString("rmi.29", interfaceHash, hash)); //$NON-NLS-1$ //$NON-NLS-2$
            }
        }

        org.apache.harmony.rmi.activation.Rmid server = (org.apache.harmony.rmi.activation.Rmid) obj;

        switch (opnum) {

        case 0: {    // activate(ActivationID, boolean)

            java.rmi.activation.ActivationID $param_ActivationID_1;
            boolean $param_boolean_2;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationID_1 = (java.rmi.activation.ActivationID) in.readObject();
                $param_boolean_2 = in.readBoolean();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            java.rmi.MarshalledObject $result = server.activate($param_ActivationID_1, $param_boolean_2);

            try {
                java.io.ObjectOutput out = call.getResultStream(true);
                out.writeObject($result);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 1: {    // activeGroup(ActivationGroupID, ActivationInstantiator, long)

            java.rmi.activation.ActivationGroupID $param_ActivationGroupID_1;
            java.rmi.activation.ActivationInstantiator $param_ActivationInstantiator_2;
            long $param_long_3;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationGroupID_1 = (java.rmi.activation.ActivationGroupID) in.readObject();
                $param_ActivationInstantiator_2 = (java.rmi.activation.ActivationInstantiator) in.readObject();
                $param_long_3 = in.readLong();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            java.rmi.activation.ActivationMonitor $result = server.activeGroup($param_ActivationGroupID_1, $param_ActivationInstantiator_2, $param_long_3);

            try {
                java.io.ObjectOutput out = call.getResultStream(true);
                out.writeObject($result);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 2: {    // activeObject(ActivationID, MarshalledObject)

            java.rmi.activation.ActivationID $param_ActivationID_1;
            java.rmi.MarshalledObject $param_MarshalledObject_2;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationID_1 = (java.rmi.activation.ActivationID) in.readObject();
                $param_MarshalledObject_2 = (java.rmi.MarshalledObject) in.readObject();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            server.activeObject($param_ActivationID_1, $param_MarshalledObject_2);

            try {
                call.getResultStream(true);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 3: {    // getActivationDesc(ActivationID)

            java.rmi.activation.ActivationID $param_ActivationID_1;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationID_1 = (java.rmi.activation.ActivationID) in.readObject();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            java.rmi.activation.ActivationDesc $result = server.getActivationDesc($param_ActivationID_1);

            try {
                java.io.ObjectOutput out = call.getResultStream(true);
                out.writeObject($result);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 4: {    // getActivationGroupDesc(ActivationGroupID)

            java.rmi.activation.ActivationGroupID $param_ActivationGroupID_1;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationGroupID_1 = (java.rmi.activation.ActivationGroupID) in.readObject();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            java.rmi.activation.ActivationGroupDesc $result = server.getActivationGroupDesc($param_ActivationGroupID_1);

            try {
                java.io.ObjectOutput out = call.getResultStream(true);
                out.writeObject($result);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 5: {    // inactiveGroup(ActivationGroupID, long)

            java.rmi.activation.ActivationGroupID $param_ActivationGroupID_1;
            long $param_long_2;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationGroupID_1 = (java.rmi.activation.ActivationGroupID) in.readObject();
                $param_long_2 = in.readLong();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            server.inactiveGroup($param_ActivationGroupID_1, $param_long_2);

            try {
                call.getResultStream(true);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 6: {    // inactiveObject(ActivationID)

            java.rmi.activation.ActivationID $param_ActivationID_1;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationID_1 = (java.rmi.activation.ActivationID) in.readObject();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            server.inactiveObject($param_ActivationID_1);

            try {
                call.getResultStream(true);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 7: {    // registerGroup(ActivationGroupDesc)

            java.rmi.activation.ActivationGroupDesc $param_ActivationGroupDesc_1;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationGroupDesc_1 = (java.rmi.activation.ActivationGroupDesc) in.readObject();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            java.rmi.activation.ActivationGroupID $result = server.registerGroup($param_ActivationGroupDesc_1);

            try {
                java.io.ObjectOutput out = call.getResultStream(true);
                out.writeObject($result);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 8: {    // registerObject(ActivationDesc)

            java.rmi.activation.ActivationDesc $param_ActivationDesc_1;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationDesc_1 = (java.rmi.activation.ActivationDesc) in.readObject();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            java.rmi.activation.ActivationID $result = server.registerObject($param_ActivationDesc_1);

            try {
                java.io.ObjectOutput out = call.getResultStream(true);
                out.writeObject($result);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 9: {    // setActivationDesc(ActivationID, ActivationDesc)

            java.rmi.activation.ActivationID $param_ActivationID_1;
            java.rmi.activation.ActivationDesc $param_ActivationDesc_2;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationID_1 = (java.rmi.activation.ActivationID) in.readObject();
                $param_ActivationDesc_2 = (java.rmi.activation.ActivationDesc) in.readObject();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            java.rmi.activation.ActivationDesc $result = server.setActivationDesc($param_ActivationID_1, $param_ActivationDesc_2);

            try {
                java.io.ObjectOutput out = call.getResultStream(true);
                out.writeObject($result);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 10: {    // setActivationGroupDesc(ActivationGroupID, ActivationGroupDesc)

            java.rmi.activation.ActivationGroupID $param_ActivationGroupID_1;
            java.rmi.activation.ActivationGroupDesc $param_ActivationGroupDesc_2;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationGroupID_1 = (java.rmi.activation.ActivationGroupID) in.readObject();
                $param_ActivationGroupDesc_2 = (java.rmi.activation.ActivationGroupDesc) in.readObject();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            java.rmi.activation.ActivationGroupDesc $result = server.setActivationGroupDesc($param_ActivationGroupID_1, $param_ActivationGroupDesc_2);

            try {
                java.io.ObjectOutput out = call.getResultStream(true);
                out.writeObject($result);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 11: {    // shutdown()

            call.releaseInputStream();

            server.shutdown();

            try {
                call.getResultStream(true);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 12: {    // unregisterGroup(ActivationGroupID)

            java.rmi.activation.ActivationGroupID $param_ActivationGroupID_1;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationGroupID_1 = (java.rmi.activation.ActivationGroupID) in.readObject();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            server.unregisterGroup($param_ActivationGroupID_1);

            try {
                call.getResultStream(true);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        case 13: {    // unregisterObject(ActivationID)

            java.rmi.activation.ActivationID $param_ActivationID_1;

            try {
                java.io.ObjectInput in = call.getInputStream();
                $param_ActivationID_1 = (java.rmi.activation.ActivationID) in.readObject();
            } catch (java.io.IOException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } catch (java.lang.ClassNotFoundException e) {
                throw new java.rmi.UnmarshalException(Messages.getString("rmi.2C"), e); //$NON-NLS-1$
            } finally {
                call.releaseInputStream();
            }

            server.unregisterObject($param_ActivationID_1);

            try {
                call.getResultStream(true);
            } catch (java.io.IOException e) {
                throw new java.rmi.MarshalException(Messages.getString("rmi.2A"), e); //$NON-NLS-1$
            }

            break;
        }

        default:
            // rmi.2B=Invalid method number: {0}
            throw new java.rmi.UnmarshalException(Messages.getString("rmi.2B", opnum)); //$NON-NLS-1$
        }
    }
}
