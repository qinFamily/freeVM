/*
*  Licensed to the Apache Software Foundation (ASF) under one or more
*  contributor license agreements.  See the NOTICE file distributed with
*  this work for additional information regarding copyright ownership.
*  The ASF licenses this file to You under the Apache License, Version 2.0
*  (the "License"); you may not use this file except in compliance with
*  the License.  You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/
/* COPYRIGHT_NOTICE */

/**
* @author Mikhail Y. Fursov
* @version $Revision$
*/

#ifndef _DRLEMINTERFACE_H_
#define _DRLEMINTERFACE_H_

#include "EMInterface.h"

namespace Jitrino {

class DrlProfilingInterface : public ProfilingInterface {
public:
    DrlProfilingInterface(EM_Handle _em, JIT_Handle _jit, EM_ProfileAccessInterface* emProfileAccess)
        : emHandle(_em), pcHandle(NULL), pcType(ProfileType_Invalid), jitHandle(_jit), profileAccessInterface(emProfileAccess), 
        jitRole(JITProfilingRole_USE), profilingEnabled(false){}

        virtual MethodProfile* getMethodProfile(MemoryManager& mm, ProfileType type, MethodDesc& md, JITProfilingRole role=JITProfilingRole_USE) const;
        virtual bool hasMethodProfile(ProfileType type, MethodDesc& md, JITProfilingRole role=JITProfilingRole_USE) const;
        virtual uint32 getProfileMethodCount(MethodDesc& md, JITProfilingRole role = JITProfilingRole_USE) const;

        virtual bool enableProfiling(PC_Handle pc, JITProfilingRole role);
        virtual bool isProfilingEnabled(ProfileType pcType, JITProfilingRole jitRole) const ;

        virtual EntryBackedgeMethodProfile* createEBMethodProfile(MemoryManager& mm, MethodDesc& md);
        virtual bool isEBProfilerInSyncMode() const;
        virtual PC_Callback_Fn* getEBProfilerSyncModeCallback() const;
        
        virtual EdgeMethodProfile* createEdgeMethodProfile(MemoryManager& mm, MethodDesc& md, uint32 numEdgeCounters, uint32* counterKeys, uint32 checkSum);

        virtual uint32 getMethodEntryThreshold() const;
        virtual uint32 getBackedgeThreshold() const;

private:
    EM_Handle emHandle;
    //Only one profile per JIT is supported
    PC_Handle pcHandle;
    ProfileType pcType;
    JIT_Handle jitHandle;
    EM_ProfileAccessInterface* profileAccessInterface;
    JITProfilingRole jitRole;
    bool profilingEnabled;
};

class DrlEntryBackedgeMethodProfile : public EntryBackedgeMethodProfile {
public:
    DrlEntryBackedgeMethodProfile(Method_Profile_Handle mph, MethodDesc& md, uint32* _entryCounter, uint32 *_backedgeCounter)
        : EntryBackedgeMethodProfile(mph, md),  entryCounter(_entryCounter), backedgeCounter(_backedgeCounter){}

        virtual uint32 getEntryExecCount() const {return *entryCounter;}
        virtual uint32 getBackedgeExecCount() const {return *backedgeCounter;}
        virtual uint32* getEntryCounter() const {return entryCounter;}
        virtual uint32* getBackedgeCounter() const {return backedgeCounter;}

private:
    uint32* entryCounter;
    uint32* backedgeCounter;
};


class DrlEdgeMethodProfile : public EdgeMethodProfile {
public:
    DrlEdgeMethodProfile (Method_Profile_Handle handle, MethodDesc& md,  EM_ProfileAccessInterface* profileAccessInterface);
    virtual uint32  getNumCounters() const;
    virtual uint32  getCheckSum() const;
    virtual uint32* getEntryCounter() const;
    virtual uint32* getCounter(uint32 key) const ;

private:
    EM_ProfileAccessInterface* profileAccessInterface;

};


}
#endif