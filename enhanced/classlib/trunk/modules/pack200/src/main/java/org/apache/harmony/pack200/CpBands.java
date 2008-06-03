/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. The ASF
 * licenses this file to You under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
package org.apache.harmony.pack200;

import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

import org.apache.bcel.classfile.ConstantClass;
import org.apache.bcel.classfile.ConstantDouble;
import org.apache.bcel.classfile.ConstantFieldref;
import org.apache.bcel.classfile.ConstantFloat;
import org.apache.bcel.classfile.ConstantInteger;
import org.apache.bcel.classfile.ConstantInterfaceMethodref;
import org.apache.bcel.classfile.ConstantLong;
import org.apache.bcel.classfile.ConstantMethodref;
import org.apache.bcel.classfile.ConstantNameAndType;
import org.apache.bcel.classfile.ConstantPool;
import org.apache.bcel.classfile.ConstantString;
import org.apache.bcel.classfile.ConstantUtf8;

public class CpBands extends BandSet {

    private final Set cp_Utf8 = new TreeSet();
    private final Set cp_Int = new TreeSet();
    private final Set cp_Float = new TreeSet();
    private final Set cp_Long = new TreeSet();
    private final Set cp_Double = new TreeSet();
    private final Set cp_String = new TreeSet();
    private final Set cp_Class = new TreeSet();
    private final Set cp_Signature = new TreeSet();
    private final Set cp_Descr = new TreeSet();
    private final Set cp_Field = new TreeSet();
    private final Set cp_Method = new TreeSet();
    private final Set cp_Imethod = new TreeSet();

    private ConstantPool currentConstantPool;
    private final Map stringsToCpClass = new HashMap();
    private final Map stringsToCpNameAndType = new HashMap();
    private final Map stringsToCpString = new HashMap();
    private final Map stringsToCpSignature = new HashMap();

    public void pack(OutputStream out) {
        // TODO Auto-generated method stub

    }

    public void setCurrentConstantPool(ConstantPool cp) {
        this.currentConstantPool = cp;
    }

    public void sortPool() {

        System.out.println("pool");
    }

    public void addConstantClass(ConstantClass constant) {
        String className = constant.getBytes(currentConstantPool);
        if(stringsToCpClass.get(className) == null) {
            CPClass cpClass = new CPClass(className);
            cp_Class.add(cpClass);
            stringsToCpClass.put(className, cpClass);
        }
    }

    public void addConstantDouble(ConstantDouble constant) {
        cp_Double.add(new Double((constant).getBytes()));
    }

    public void addConstantFieldref(ConstantFieldref constant) {
        ConstantFieldref cf = constant;
        ConstantNameAndType cnat = (ConstantNameAndType) currentConstantPool
                .getConstant(cf.getNameAndTypeIndex());
        CPNameAndType nat = new CPNameAndType(cnat.getName(currentConstantPool),
                cnat.getSignature(currentConstantPool));
        cp_Signature.add(cnat.getSignature(currentConstantPool));
        cp_Field.add(new MethodOrField(cf.getClass(currentConstantPool), nat));
    }

    public void addConstantFloat(ConstantFloat constant) {
        cp_Float.add(new Float((constant).getBytes()));
    }

    public void addConstantInteger(ConstantInteger constant) {
        cp_Int.add(new Integer((constant).getBytes()));
    }

    public void addConstantInterfaceMethodref(
            ConstantInterfaceMethodref constant) {
        ConstantNameAndType cnat = (ConstantNameAndType) currentConstantPool
                .getConstant(constant.getNameAndTypeIndex());
        String signature = cnat.getSignature(currentConstantPool);
        cp_Signature.add(signature);
        CPNameAndType nat = new CPNameAndType(cnat.getName(currentConstantPool), signature);
        cp_Imethod.add(new MethodOrField(constant.getClass(currentConstantPool), nat));
    }

    public void addConstantLong(ConstantLong constant) {
        cp_Long.add(new Long((constant).getBytes()));
    }

    public void addConstantMethodref(ConstantMethodref constant) {
        ConstantNameAndType cnat = (ConstantNameAndType) currentConstantPool
                .getConstant(constant.getNameAndTypeIndex());
        String signature = cnat.getSignature(currentConstantPool);
        cp_Signature.add(signature);
        CPNameAndType nat = new CPNameAndType(cnat.getName(currentConstantPool),
                signature);
        cp_Method.add(new MethodOrField(constant.getClass(currentConstantPool),
                nat));
    }

    public void addConstantNameAndType(ConstantNameAndType constant) {
        String name = constant.getName(currentConstantPool);
        String signature = constant.getSignature(currentConstantPool);
        cp_Signature.add(signature);
        CPNameAndType nameAndType = new CPNameAndType(name,
                signature);
        stringsToCpNameAndType.put(name + ":" + signature, nameAndType);
        cp_Descr.add(nameAndType);
    }

    public void addConstantString(ConstantString constant) {
        String string = constant.getBytes(currentConstantPool);
        if(stringsToCpString.get(string) == null) {
            CPString cpString = new CPString(string);
            cp_String.add(cpString);
            stringsToCpString.put(string, cpString);
        }
    }

    public void addConstantUtf8(ConstantUtf8 constant) {
        cp_Utf8.add((constant).getBytes());
    }

    public void addDesc(String name, String signature) {
        cp_Signature.add(signature);
        cp_Descr.add(new CPNameAndType(name, signature));
    }

    public void addSignature(String signature) {
        cp_Signature.add(signature);
    }

    public CPClass getCPClass(String className) {
        if(stringsToCpClass.get(className) == null) {
            throw new RuntimeException("null");
        }
        return (CPClass) stringsToCpClass.get(className);
    }

    public CPNameAndType getCPNameAndType(String name, String signature) {
        String str = name + ":" + signature;
        if(stringsToCpNameAndType.get(str) == null) {
            throw new RuntimeException("null");
        }
        return (CPNameAndType) stringsToCpNameAndType.get(str);
    }

}
