;
;  Licensed to the Apache Software Foundation (ASF) under one or more
;  contributor license agreements.  See the NOTICE file distributed with
;  this work for additional information regarding copyright ownership.
;  The ASF licenses this file to You under the Apache License, Version 2.0
;  (the "License"); you may not use this file except in compliance with
;  the License.  You may obtain a copy of the License at
;
;     http://www.apache.org/licenses/LICENSE-2.0
;
;  Unless required by applicable law or agreed to in writing, software
;  distributed under the License is distributed on an "AS IS" BASIS,
;  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;  See the License for the specific language governing permissions and
;  limitations under the License.
;
.class public org/apache/harmony/drlvm/tests/regression/h2808/finTest2
.super org/apache/harmony/drlvm/tests/regression/h2808/finTest1
;
; standard initializer
.method public <init>()V
   aload_0
   invokespecial finTest1/<init>()V
   return
.end method 
;
.method public final testMethod()I
  .limit locals 2
  .limit stack 2
  sipush 2
  ireturn
.end method

