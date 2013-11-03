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
; Author: Maxim V. Makarov
; Version: $Revision: 1.2 $
;

.class public org/apache/harmony/vts/test/vm/jvms/instructions/reftypes/putstatic/putstatic04/putstatic0405/putstatic0405p
.super java/lang/Object

.field public static testField F

.field public static testField1 D
;
; standard initializer
.method public <init>()V
   aload_0
   invokespecial java/lang/Object/<init>()V
   return
.end method

;
; test method
.method public test([Ljava/lang/String;)I
   .limit stack 5
   .limit locals 2

   ldc 3.4028235E38 ; MAX_VALUE
   dup
   putstatic org/apache/harmony/vts/test/vm/jvms/instructions/reftypes/putstatic/putstatic04/putstatic0405/putstatic0405p/testField F 
   getstatic org/apache/harmony/vts/test/vm/jvms/instructions/reftypes/putstatic/putstatic04/putstatic0405/putstatic0405p/testField F 

   fcmpl
   ifne Fail

   ldc2_w 1.7976931348623157E308 ; MAX_VALUE
   dup2
   putstatic org/apache/harmony/vts/test/vm/jvms/instructions/reftypes/putstatic/putstatic04/putstatic0405/putstatic0405p/testField1 D 
   getstatic org/apache/harmony/vts/test/vm/jvms/instructions/reftypes/putstatic/putstatic04/putstatic0405/putstatic0405p/testField1 D 

   dcmpl
   ifne Fail

   ; test passed
   sipush 104
   ireturn

Fail:

   ;test failed
   sipush 105
   ireturn
.end method

;
; standard main function
.method public static main([Ljava/lang/String;)V
  .limit stack 2
  .limit locals 1

  new org/apache/harmony/vts/test/vm/jvms/instructions/reftypes/putstatic/putstatic04/putstatic0405/putstatic0405p
  dup
  invokespecial org/apache/harmony/vts/test/vm/jvms/instructions/reftypes/putstatic/putstatic04/putstatic0405/putstatic0405p/<init>()V
  aload_0
  invokevirtual org/apache/harmony/vts/test/vm/jvms/instructions/reftypes/putstatic/putstatic04/putstatic0405/putstatic0405p/test([Ljava/lang/String;)I
  invokestatic java/lang/System/exit(I)V

  return
.end method
