/*
 * RMI stub class
 * for class org.apache.harmony.rmi.test.MyRemoteObject
 * Compatible with stub protocol version 1.2
 *
 * Generated by DRL RMI Compiler (rmic).
 *
 * DO NOT EDIT!!!
 * Contents subject to change without notice!
 */
package org.apache.harmony.rmi.test;


public final class MyRemoteObject_Stub extends java.rmi.server.RemoteStub
        implements org.apache.harmony.rmi.test.MyRemoteInterface, java.rmi.Remote {

    private static final long serialVersionUID = 2;

    private static java.lang.reflect.Method $method_test_Array_Void_0;
    private static java.lang.reflect.Method $method_test_BooleanStringRemote_Void_1;
    private static java.lang.reflect.Method $method_test_Error_2;
    private static java.lang.reflect.Method $method_test_Exception_3;
    private static java.lang.reflect.Method $method_test_IntArray_Void_4;
    private static java.lang.reflect.Method $method_test_Int_Void_5;
    private static java.lang.reflect.Method $method_test_Long_Long_6;
    private static java.lang.reflect.Method $method_test_Proxy_Void_7;
    private static java.lang.reflect.Method $method_test_RemoteArray_Void_8;
    private static java.lang.reflect.Method $method_test_RemoteException_9;
    private static java.lang.reflect.Method $method_test_RemoteRemote_Remote_10;
    private static java.lang.reflect.Method $method_test_RemoteString_Void_11;
    private static java.lang.reflect.Method $method_test_Remote_Remote_12;
    private static java.lang.reflect.Method $method_test_Remote_Void_13;
    private static java.lang.reflect.Method $method_test_RuntimeException_14;
    private static java.lang.reflect.Method $method_test_String_String_15;
    private static java.lang.reflect.Method $method_test_String_Void_16;
    private static java.lang.reflect.Method $method_test_UnicastRemoteObject_Void_17;
    private static java.lang.reflect.Method $method_test_Void_Array_18;
    private static java.lang.reflect.Method $method_test_Void_Int_19;
    private static java.lang.reflect.Method $method_test_Void_IntArray_20;
    private static java.lang.reflect.Method $method_test_Void_Remote_21;
    private static java.lang.reflect.Method $method_test_Void_RemoteArray_22;
    private static java.lang.reflect.Method $method_test_Void_String_23;

    static {
        try {
            $method_test_Array_Void_0 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Array_Void", new java.lang.Class[] {java.lang.String[].class});
            $method_test_BooleanStringRemote_Void_1 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_BooleanStringRemote_Void", new java.lang.Class[] {boolean.class, java.lang.String.class, java.rmi.Remote.class});
            $method_test_Error_2 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Error", new java.lang.Class[] {});
            $method_test_Exception_3 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Exception", new java.lang.Class[] {});
            $method_test_IntArray_Void_4 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_IntArray_Void", new java.lang.Class[] {int[].class});
            $method_test_Int_Void_5 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Int_Void", new java.lang.Class[] {int.class});
            $method_test_Long_Long_6 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Long_Long", new java.lang.Class[] {long.class});
            $method_test_Proxy_Void_7 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Proxy_Void", new java.lang.Class[] {java.lang.Object.class});
            $method_test_RemoteArray_Void_8 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_RemoteArray_Void", new java.lang.Class[] {java.rmi.Remote[].class});
            $method_test_RemoteException_9 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_RemoteException", new java.lang.Class[] {});
            $method_test_RemoteRemote_Remote_10 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_RemoteRemote_Remote", new java.lang.Class[] {java.rmi.Remote.class, java.rmi.Remote.class});
            $method_test_RemoteString_Void_11 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_RemoteString_Void", new java.lang.Class[] {java.rmi.Remote.class, java.lang.String.class});
            $method_test_Remote_Remote_12 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Remote_Remote", new java.lang.Class[] {java.rmi.Remote.class});
            $method_test_Remote_Void_13 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Remote_Void", new java.lang.Class[] {java.rmi.Remote.class});
            $method_test_RuntimeException_14 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_RuntimeException", new java.lang.Class[] {});
            $method_test_String_String_15 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_String_String", new java.lang.Class[] {java.lang.String.class});
            $method_test_String_Void_16 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_String_Void", new java.lang.Class[] {java.lang.String.class});
            $method_test_UnicastRemoteObject_Void_17 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_UnicastRemoteObject_Void", new java.lang.Class[] {java.rmi.Remote.class});
            $method_test_Void_Array_18 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Void_Array", new java.lang.Class[] {});
            $method_test_Void_Int_19 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Void_Int", new java.lang.Class[] {});
            $method_test_Void_IntArray_20 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Void_IntArray", new java.lang.Class[] {});
            $method_test_Void_Remote_21 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Void_Remote", new java.lang.Class[] {});
            $method_test_Void_RemoteArray_22 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Void_RemoteArray", new java.lang.Class[] {});
            $method_test_Void_String_23 = org.apache.harmony.rmi.test.MyRemoteInterface.class.getMethod("test_Void_String", new java.lang.Class[] {});
        } catch (java.lang.NoSuchMethodException e) {
            throw new java.lang.NoSuchMethodError(
                    "Stub class initialization failed: org.apache.harmony.rmi.test.MyRemoteObject_Stub");
        }
    }

    public MyRemoteObject_Stub(java.rmi.server.RemoteRef ref) {
        super(ref);
    }

    // Implementation of test_Array_Void(String[])
    public void test_Array_Void(java.lang.String[] $param_arrayOf_String_1)
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_Array_Void_0, new java.lang.Object[] {$param_arrayOf_String_1}, -2243646371196886499L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_BooleanStringRemote_Void(boolean, String, Remote)
    public void test_BooleanStringRemote_Void(boolean $param_boolean_1, java.lang.String $param_String_2, java.rmi.Remote $param_Remote_3)
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_BooleanStringRemote_Void_1, new java.lang.Object[] {new java.lang.Boolean($param_boolean_1), $param_String_2, $param_Remote_3}, 7275758542885426298L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Error()
    public void test_Error()
            throws java.rmi.RemoteException, org.apache.harmony.rmi.test.MyException {
        try {
            ref.invoke(this, $method_test_Error_2, null, 4293752454580169568L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (org.apache.harmony.rmi.test.MyException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Exception()
    public void test_Exception()
            throws java.rmi.RemoteException, org.apache.harmony.rmi.test.MyException {
        try {
            ref.invoke(this, $method_test_Exception_3, null, -3748746352694661558L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (org.apache.harmony.rmi.test.MyException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_IntArray_Void(int[])
    public void test_IntArray_Void(int[] $param_arrayOf_int_1)
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_IntArray_Void_4, new java.lang.Object[] {$param_arrayOf_int_1}, 7755952240160380354L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Int_Void(int)
    public void test_Int_Void(int $param_int_1)
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_Int_Void_5, new java.lang.Object[] {new java.lang.Integer($param_int_1)}, 5910792822519007408L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Long_Long(long)
    public long test_Long_Long(long $param_long_1)
            throws java.rmi.RemoteException {
        try {
            java.lang.Object $result = ref.invoke(this, $method_test_Long_Long_6, new java.lang.Object[] {new java.lang.Long($param_long_1)}, -3247234805189459325L);
            return ((java.lang.Long) $result).longValue();
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Proxy_Void(Object)
    public void test_Proxy_Void(java.lang.Object $param_Object_1)
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_Proxy_Void_7, new java.lang.Object[] {$param_Object_1}, 1862985695118685397L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_RemoteArray_Void(Remote[])
    public void test_RemoteArray_Void(java.rmi.Remote[] $param_arrayOf_Remote_1)
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_RemoteArray_Void_8, new java.lang.Object[] {$param_arrayOf_Remote_1}, -1903079572015069111L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_RemoteException()
    public void test_RemoteException()
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_RemoteException_9, null, -4280707563916630440L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_RemoteRemote_Remote(Remote, Remote)
    public java.rmi.Remote test_RemoteRemote_Remote(java.rmi.Remote $param_Remote_1, java.rmi.Remote $param_Remote_2)
            throws java.rmi.RemoteException {
        try {
            java.lang.Object $result = ref.invoke(this, $method_test_RemoteRemote_Remote_10, new java.lang.Object[] {$param_Remote_1, $param_Remote_2}, 5877054312994691713L);
            return ((java.rmi.Remote) $result);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_RemoteString_Void(Remote, String)
    public void test_RemoteString_Void(java.rmi.Remote $param_Remote_1, java.lang.String $param_String_2)
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_RemoteString_Void_11, new java.lang.Object[] {$param_Remote_1, $param_String_2}, 8074777281465601732L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Remote_Remote(Remote)
    public java.rmi.Remote test_Remote_Remote(java.rmi.Remote $param_Remote_1)
            throws java.rmi.RemoteException {
        try {
            java.lang.Object $result = ref.invoke(this, $method_test_Remote_Remote_12, new java.lang.Object[] {$param_Remote_1}, -4114253240958026174L);
            return ((java.rmi.Remote) $result);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Remote_Void(Remote)
    public void test_Remote_Void(java.rmi.Remote $param_Remote_1)
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_Remote_Void_13, new java.lang.Object[] {$param_Remote_1}, 1361325135665590515L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_RuntimeException()
    public void test_RuntimeException()
            throws java.rmi.RemoteException, org.apache.harmony.rmi.test.MyException {
        try {
            ref.invoke(this, $method_test_RuntimeException_14, null, -127135464990399242L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (org.apache.harmony.rmi.test.MyException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_String_String(String)
    public java.lang.String test_String_String(java.lang.String $param_String_1)
            throws java.rmi.RemoteException {
        try {
            java.lang.Object $result = ref.invoke(this, $method_test_String_String_15, new java.lang.Object[] {$param_String_1}, -3452752672819966636L);
            return ((java.lang.String) $result);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_String_Void(String)
    public void test_String_Void(java.lang.String $param_String_1)
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_String_Void_16, new java.lang.Object[] {$param_String_1}, 3320885435564229415L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_UnicastRemoteObject_Void(Remote)
    public void test_UnicastRemoteObject_Void(java.rmi.Remote $param_Remote_1)
            throws java.rmi.RemoteException {
        try {
            ref.invoke(this, $method_test_UnicastRemoteObject_Void_17, new java.lang.Object[] {$param_Remote_1}, 144033458432082551L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Void_Array()
    public java.lang.String[] test_Void_Array()
            throws java.rmi.RemoteException {
        try {
            java.lang.Object $result = ref.invoke(this, $method_test_Void_Array_18, null, -5057311836623655794L);
            return ((java.lang.String[]) $result);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Void_Int()
    public int test_Void_Int()
            throws java.rmi.RemoteException {
        try {
            java.lang.Object $result = ref.invoke(this, $method_test_Void_Int_19, null, 1875101440413673248L);
            return ((java.lang.Integer) $result).intValue();
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Void_IntArray()
    public int[] test_Void_IntArray()
            throws java.rmi.RemoteException {
        try {
            java.lang.Object $result = ref.invoke(this, $method_test_Void_IntArray_20, null, 6041902044718675347L);
            return ((int[]) $result);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Void_Remote()
    public java.rmi.Remote test_Void_Remote()
            throws java.rmi.RemoteException {
        try {
            java.lang.Object $result = ref.invoke(this, $method_test_Void_Remote_21, null, -1711397921290492632L);
            return ((java.rmi.Remote) $result);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Void_RemoteArray()
    public java.rmi.Remote[] test_Void_RemoteArray()
            throws java.rmi.RemoteException {
        try {
            java.lang.Object $result = ref.invoke(this, $method_test_Void_RemoteArray_22, null, 4456231954737882310L);
            return ((java.rmi.Remote[]) $result);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }

    // Implementation of test_Void_String()
    public java.lang.String test_Void_String()
            throws java.rmi.RemoteException {
        try {
            java.lang.Object $result = ref.invoke(this, $method_test_Void_String_23, null, -5382831137617544633L);
            return ((java.lang.String) $result);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("Undeclared checked exception", e);
        }
    }
}
