# Copyright 2003, 2005 The Apache Software Foundation or its licensors, as applicable
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# Makefile for module 'sig'
#

!include <$(HY_HDK)\build\make\defines.mak>

LIBBASE=hysig
DLLNAME=..\$(LIBBASE).dll
LIBNAME=$(LIBPATH)$(LIBBASE).lib
BUILDFILES = $(SHAREDSUB)hysig_copyright.obj hysigwin.obj
HYLDFLAGS = $(HYLDFLAGS) -def:$(LIBBASE).def
VIRTFILES = $(LIBBASE).res
MDLLIBFILES =
DLLBASE=0x13100000
COMMENT=/comment:"Signal handling support DLL. (c) Copyright 1993, 2005 The Apache Software Foundation or its licensors, as applicable."

!include <$(HY_HDK)\build\make\rules.mak>
