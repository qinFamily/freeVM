;    Copyright 2005-2006 The Apache Software Foundation or its licensors, as applicable
;
;    Licensed under the Apache License, Version 2.0 (the "License");
;    you may not use this file except in compliance with the License.
;    You may obtain a copy of the License at
;
;       http://www.apache.org/licenses/LICENSE-2.0
;
;    Unless required by applicable law or agreed to in writing, software
;    distributed under the License is distributed on an "AS IS" BASIS,
;    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;
;    See the License for the specific language governing permissions and
;    limitations under the License.

;
; Author: Maxim N. Kurzenev
; Version: $Revision: 1.1 $
;

.source threads09701p.j
.class public org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p
.super org/apache/harmony/vts/test/vm/jvms/threads/share/InterruptibleTest

.field  a I

.method public <init>()V
    .limit stack 1
    .limit locals 1

	aload_0
	invokespecial org/apache/harmony/vts/test/vm/jvms/threads/share/InterruptibleTest/<init>()V
	return
.end method

.method public testTimed(I)I
    .limit stack 4
    .limit locals 4

    .throws java/lang/InterruptedException

    ;        boolean passed = true;
	iconst_1
	istore_2
    ;        Thread t1 = new threads09701p$1()
	new org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p$1
	dup
	aload_0
	invokespecial org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p$1/<init>(Lorg/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p;)V
	astore_3
    ;        new Interruptor(this, delay).start();
	new org/apache/harmony/vts/test/vm/jvms/threads/share/Interruptor
	dup
	aload_0
	iload_1
	invokespecial org/apache/harmony/vts/test/vm/jvms/threads/share/Interruptor/<init>(Lorg/apache/harmony/vts/test/vm/jvms/threads/share/InterruptibleTest;I)V
	invokevirtual org/apache/harmony/vts/test/vm/jvms/threads/share/Interruptor/start()V
    ;        synchronized (this) {
	aload_0
	monitorenter
    ;            t1.start();
	aload_3
	invokevirtual java/lang/Thread/start()V
    ;            a = 0;
	aload_0
	iconst_0
	putfield org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p/a I
LoopStart:
    ;            while (!interrupted()) {
	aload_0
	invokevirtual org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p/interrupted()Z
	ifne Cleanup
    ;                if (a != 0) {
	aload_0
	getfield org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p/a I
	ifeq Check_OK
    ;                    passed = false;
    ;                    break;
    ;                }
	iconst_0
	istore_2
	goto Cleanup
Check_OK:
    ;                a--;
	aload_0
	dup
	getfield org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p/a I
	iconst_1
	isub
	putfield org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p/a I
    ;                this.notifyAll();
	aload_0
	invokevirtual java/lang/Object/notifyAll()V
    ;                this.wait();
    ;            } // while
	aload_0
	invokevirtual java/lang/Object/wait()V
	goto LoopStart
Cleanup:
    ;        } // synchronized block
	aload_0
	monitorexit
    ;        t1.interrupt();
	aload_3
	invokevirtual java/lang/Thread/interrupt()V
    ;        return passed ? 104 : 105;
	iload_2
	ifeq Failed
	bipush 104
	ireturn
Failed:
	bipush 105
	ireturn
.end method

.method public static main([Ljava/lang/String;)V
    .limit stack 2
    .limit locals 1

    .throws java/lang/InterruptedException

	new org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p
	dup
	invokespecial org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p/<init>()V
	aload_0
	invokevirtual org/apache/harmony/vts/test/vm/jvms/threads/threads097/threads09701/threads09701p/test([Ljava/lang/String;)I
	invokestatic java/lang/System/exit(I)V
	return
.end method
