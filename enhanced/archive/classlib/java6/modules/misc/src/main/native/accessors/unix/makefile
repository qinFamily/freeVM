# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include $(HY_HDK)/build/make/defines.mk

CFLAGS += -fpic
INCLUDES += -I$(SHARED)common

BUILDFILES = \
  $(SHAREDSUB)org_apache_harmony_misc_accessors_ArrayAccessorImpl.o \
  $(SHAREDSUB)org_apache_harmony_misc_accessors_Malloc.o \
  $(SHAREDSUB)org_apache_harmony_misc_accessors_MemoryAccessorNativeImpl.o \
  $(SHAREDSUB)org_apache_harmony_misc_accessors_ObjectAccessorImpl.o \
  $(SHAREDSUB)org_apache_harmony_misc_accessors_StringAccessorImpl.o


MDLLIBFILES += \
	$(LIBPATH)libhyzip.a $(DLLPATH)libhyzlib.so \
	$(LIBPATH)libhypool.a $(LIBPATH)libhyfdlibm.a $(LIBPATH)libvmi.so

DLLNAME=../libaccessors.so
EXPNAME=HYMISC_0.1

include $(HY_HDK)/build/make/rules.mk
