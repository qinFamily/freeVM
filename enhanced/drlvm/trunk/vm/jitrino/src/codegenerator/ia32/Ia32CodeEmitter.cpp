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
/**
 * @author Vyacheslav P. Shakin
 * @version $Revision: 1.12.14.1.4.3 $
 */

#include "Ia32IRManager.h"
#include "CGSupport.h"
#include "Log.h"
#include "Ia32RuntimeInterface.h"
#include "Ia32Printer.h"
#include "Ia32RegAllocCheck.h"
#include "Ia32GCMap.h"
#include "Ia32BCMap.h"
#include "EMInterface.h"
#include "InlineInfo.h"

namespace Jitrino
{
namespace Ia32 
{

/**
    class CodeEmitter
    
*/
class CompiledMethodInfo;
class CodeEmitter : public SessionAction {
public:
    CodeEmitter()
        :memoryManager(0x1000, "CodeEmitter"),
        exceptionHandlerInfos(memoryManager), constantAreaLayout(memoryManager),
        traversalInfo(memoryManager), methodLocationMap(memoryManager), 
        entryExitMap(memoryManager), instSizeMap(memoryManager)
    {
    }


    CodeBlockHeat getCodeSectionHeat(uint32 sectionID)const;

protected:
    uint32 getNeedInfo()const{ return NeedInfo_LivenessInfo; }
    uint32 getSideEffects()const{ return 0; }

    void runImpl();
    bool verify(bool force=false);

    //------------------------------------------------------------------------------------
    void emitCode();
    void registerExceptionHandlers();
    void registerExceptionRegion(void * regionStart, void * regionEnd, Node * regionDispatchNode);
    void packCode();
    void postPass();
    void registerDirectCall(MethodDesc * md, void * instStartAddr);
    void registerInlineInfoOffsets( void );

    void orderNodesAndMarkInlinees(StlList<MethodMarkerPseudoInst*>& container, 
        Node * node, bool isForward);
    void reportCompiledInlinees();
    void reportInlinedMethod(CompiledMethodInfo* methInfo, MethodMarkerPseudoInst* methEntryInst);

    //------------------------------------------------------------------------------------
    class ConstantAreaLayout
    {
    public:
        ConstantAreaLayout(MemoryManager& mm)
            :memoryManager(mm), items(memoryManager){};

        void collectItems();
        void calculateItemOffsets();
        void doLayout(IRManager*);
        void finalizeSwitchTables();

    protected:
        IRManager*                      irManager;
        MemoryManager&                  memoryManager;
        StlVector<ConstantAreaItem*>    items;

        POINTER_SIZE_INT                dataSize;

        const static POINTER_SIZE_INT blockAlignment=16;
    private:

    };


    //------------------------------------------------------------------------------------
    struct ExceptionHandlerInfo
    {
        void *          regionStart;
        void *          regionEnd;
        void *          handlerAddr;
        ObjectType *    exceptionType;
        bool            exceptionObjectIsDead;
        
        ExceptionHandlerInfo(
            void *          _regionStart,
            void *          _regionEnd,
            void *          _handlerAddr, 
            ObjectType *    _exceptionType, 
            bool            _exceptionObjectIsDead=false
            ):
            regionStart(_regionStart),
            regionEnd(_regionEnd),
            handlerAddr(_handlerAddr),
            exceptionType(_exceptionType),
            exceptionObjectIsDead(_exceptionObjectIsDead){}

    };
    
    // bc to native map stuff
    VectorHandler* bc2LIRMapHandler;

    MemoryManager                   memoryManager;
    StlVector<ExceptionHandlerInfo> exceptionHandlerInfos;
    ConstantAreaLayout              constantAreaLayout;
    StlVector<int> traversalInfo;
    StlMap<MethodMarkerPseudoInst*, CompiledMethodInfo* > methodLocationMap;
    StlMap<MethodMarkerPseudoInst*, MethodMarkerPseudoInst* > entryExitMap;
    StlMap<POINTER_SIZE_INT, unsigned> instSizeMap;
};

typedef StlMap<POINTER_SIZE_INT, uint16> LocationMap;

class CompiledMethodInfo {
public:
    CompiledMethodInfo(MemoryManager& mm, POINTER_SIZE_INT addr, MethodMarkerPseudoInst* outerEntry, uint32 idpth):
        memoryManager(mm),
        locationMap(memoryManager),
        codeSize(0),
        codeAddr(addr),
        outerMethodEntry(outerEntry),
        inlineDepth(idpth)
    {}
    uint32 getCodeSize() { return codeSize; }
    uint32 getInlineDepth() { return inlineDepth; }
    POINTER_SIZE_INT getCodeAddr() { return codeAddr; }
    MethodMarkerPseudoInst* getOuterMethodEntry() { return outerMethodEntry; }

protected:
    friend class CodeEmitter;
    MemoryManager& memoryManager;
    LocationMap locationMap;
    uint32 codeSize;
    POINTER_SIZE_INT codeAddr;
    MethodMarkerPseudoInst* outerMethodEntry;
    // inlineDepth == 1 means that CompiledMethod is inlined into irManager->getMethodDesc()
    uint32 inlineDepth;

    void addCodeSize(uint32 size) { codeSize += size; }

    void includeInst(Inst* inst, uint64 bcOffset) {
        if( inst->hasKind(Inst::Kind_PseudoInst)) {
            return;
        } else {
            addCodeSize(inst->getCodeSize());
        }

        POINTER_SIZE_INT instStartAddr = (POINTER_SIZE_INT) inst->getCodeStartAddr();
        assert(!locationMap.has(instStartAddr));
        locationMap[instStartAddr] = (uint16)bcOffset;
    }
private:

};


static ActionFactory<CodeEmitter> _emitter("emitter");

//========================================================================================
// class CodeEmitter::ConstantAreaLayout
//========================================================================================
//________________________________________________________________________________________
void CodeEmitter::ConstantAreaLayout::collectItems()
{
    for (uint32 i=0, n=irManager->getOpndCount(); i<n; ++i){
        Opnd * opnd=irManager->getOpnd(i);
        Opnd::RuntimeInfo * ri=NULL;
        if (opnd->isPlacedIn(OpndKind_Mem)&&opnd->getMemOpndKind()==MemOpndKind_ConstantArea){
            Opnd * addrOpnd=opnd->getMemOpndSubOpnd(MemOpndSubOpndKind_Displacement);
#ifndef _EM64T_
            ri=addrOpnd->getRuntimeInfo();
            assert(ri->getKind()==Opnd::RuntimeInfo::Kind_ConstantAreaItem);
#else
            if(addrOpnd) {
                ri=addrOpnd->getRuntimeInfo();
                assert(ri->getKind()==Opnd::RuntimeInfo::Kind_ConstantAreaItem);
            }
#endif
        }else if (opnd->isPlacedIn(OpndKind_Imm)){
            ri=opnd->getRuntimeInfo();
            if (ri && ri->getKind()!=Opnd::RuntimeInfo::Kind_ConstantAreaItem)
                ri=NULL;
        }
        if (ri!=NULL){
            ConstantAreaItem * item=(ConstantAreaItem *)ri->getValue(0);
            if (item->getAddress()!=(void*)1){
                items.push_back(item);
                item->setAddress((void*)1);
            }
        }
    }
}

//________________________________________________________________________________________
void CodeEmitter::ConstantAreaLayout::calculateItemOffsets()
{
    POINTER_SIZE_INT offset=0;
    for (size_t i=0, n=items.size(); i<n; ++i){
        ConstantAreaItem * item=items[i];
        POINTER_SIZE_INT size=item->getSize();
        POINTER_SIZE_INT alignment=size; 
        if (size<=4)
            alignment=4;
        else if (size<=8)
            alignment=8;
        else
            alignment=16;
        offset=(offset+alignment-1)&~(alignment-1);
        item->setAddress((void*)offset);
        offset+=size;
    }
    dataSize=offset;
}

//________________________________________________________________________________________
void CodeEmitter::ConstantAreaLayout::doLayout(IRManager* irm)
{
    irManager = irm;
    collectItems();
    calculateItemOffsets();

    POINTER_SIZE_INT dataBlock = (POINTER_SIZE_INT)irManager->getCompilationInterface()
        .allocateDataBlock(dataSize + blockAlignment*2, blockAlignment);
    dataBlock=(dataBlock+blockAlignment-1)&~(blockAlignment-1);
    assert(dataBlock % blockAlignment == 0);

    for (size_t i=0, n=items.size(); i<n; ++i){
        ConstantAreaItem * item=items[i];
        POINTER_SIZE_INT offset=(POINTER_SIZE_INT)item->getAddress();
        item->setAddress((void*)(dataBlock+offset));
        if (!item->hasKind(ConstantAreaItem::Kind_SwitchTableConstantAreaItem))
            memcpy(item->getAddress(), item->getValue(), item->getSize());
    }   
}

//________________________________________________________________________________________
void CodeEmitter::ConstantAreaLayout::finalizeSwitchTables()
{
#ifdef _DEBUG
    const Nodes& nodes = irManager->getFlowGraph()->getNodes();
#endif
    for (size_t i=0, n=items.size(); i<n; ++i){
        ConstantAreaItem * item=items[i];
        if (item->hasKind(ConstantAreaItem::Kind_SwitchTableConstantAreaItem)){
            void ** table = (void **)item->getAddress();
            Node** bbs=(Node **)item->getValue();
            uint32 nbb=(uint32)item->getSize()/sizeof(Node*);
            for (uint32 ibb=0; ibb<nbb; ibb++){
                BasicBlock* bb=(BasicBlock*)bbs[ibb];
                assert(bb!=NULL);
                assert(std::find(nodes.begin(), nodes.end(), bb)!=nodes.end());
                table[ibb]=bb->getCodeStartAddr();
            }
        }
    }   
}


//========================================================================================
// class CodeEmitter
//========================================================================================

//________________________________________________________________________________________
void CodeEmitter::runImpl()
{
    if (irManager->getCompilationInterface().isBCMapInfoRequired()) {
        MethodDesc* meth = irManager->getCompilationInterface().getMethodToCompile();
        bc2LIRMapHandler = new(memoryManager) VectorHandler(bcOffset2LIRHandlerName, meth);
    }

    irManager->setInfo("inlineInfo", new(irManager->getMemoryManager()) InlineInfoMap(irManager->getMemoryManager()));
    constantAreaLayout.doLayout(irManager);
    irManager->resolveRuntimeInfo();
    emitCode();
    packCode();
    postPass();
    constantAreaLayout.finalizeSwitchTables();
    registerExceptionHandlers();
    registerInlineInfoOffsets();
    if (irManager->getCompilationInterface().isCompileLoadEventRequired()) {
        reportCompiledInlinees();
    }
}
//________________________________________________________________________________________
void CodeEmitter::registerInlineInfoOffsets() 
{
    InlineInfoMap * inlineMap = (InlineInfoMap *)irManager->getInfo("inlineInfo");
    const Nodes& nodes = irManager->getFlowGraph()->getNodes();
    for (Nodes::const_iterator it = nodes.begin(), end = nodes.end(); it!=end; ++it) {
        Node* node = *it;
        if (node->isBlockNode()){
            for (Inst * inst=(Inst*)node->getFirstInst(); inst!=NULL; inst=inst->getNextInst()){
                if(inst->getMnemonic() == Mnemonic_CALL) {
                    CallInst* callinst = (CallInst*)inst;
                    if ( callinst->getInlineInfo() ) {
                        Log::out() << "callinstr, at offset=" << (POINTER_SIZE_INT)inst->getCodeStartAddr()+inst->getCodeSize() << std::endl;
                        Log::out() << "has inline info:" << std::endl;
                        callinst->getInlineInfo()->printLevels(Log::out());
                        // report offset 1 bundle forward
                        POINTER_SIZE_INT offset = (POINTER_SIZE_INT)inst->getCodeStartAddr() + inst->getCodeSize() - (POINTER_SIZE_INT)irManager->getCodeStartAddr();
                        assert(fit32(offset));
                        inlineMap->registerOffset((uint32)offset, callinst->getInlineInfo());
                    }
                    Log::out() << std::endl;
                }
            }
        }
    }
}

//________________________________________________________________________________________
bool CodeEmitter::verify (bool force)
{   
    bool failed=false;
    if (force || getVerificationLevel() >= 1)
    {
        irManager->updateLivenessInfo();
        for (uint32 i=0, n=irManager->getOpndCount(); i<n; i++){
            Opnd * opnd=irManager->getOpnd(i);
            if (!opnd->hasAssignedPhysicalLocation()){
                VERIFY_OUT("Unassigned operand: " << opnd << ::std::endl);
                failed=true;
            }
        }
    }
    return !failed;
}   

//________________________________________________________________________________________
void CodeEmitter::emitCode( void ) {

    // irManager->getMaxInstId() - is max possible number of instructions
    // irManager->getMaxNodeId() - to handle number of basic blocks, when jmp targets alignment used
    // as the current scheme only process basic blocks as jmp targets
    const unsigned maxMethodSize = (irManager->getMaxInstId() + irManager->getFlowGraph()->getMaxNodeId())*MAX_NATIVE_INST_SIZE;

    //
    uint8 * codeStreamStart = (uint8*)malloc( maxMethodSize );
    //+---- free() below
    //|
    //v
    //
#ifdef _DEBUG
    memset(codeStreamStart, 0xCC, maxMethodSize);
#endif

    int alignment = getIntArg("align", 0);
    
    LoopTree * lt = irManager->getFlowGraph()->getLoopTree();

    uint8 * ip = codeStreamStart;
    for( BasicBlock * bb = (BasicBlock*)irManager->getFlowGraph()->getEntryNode(); bb != NULL; bb=bb->getLayoutSucc()) {

        
        if (alignment && lt->isLoopHeader(bb) && ((POINTER_SIZE_INT)ip & (alignment-1))) {
            unsigned align = alignment - (unsigned)((POINTER_SIZE_INT)ip & (alignment-1));
            ip = (uint8*)EncoderBase::nops((char*)ip, align);
        }

        uint8 * blockStartIp = ip;
        assert(fit32(blockStartIp-codeStreamStart));
        bb->setCodeOffset( (uint32)(blockStartIp-codeStreamStart) );
        for (Inst* inst = (Inst*)bb->getFirstInst(); inst!=NULL; inst = inst->getNextInst()) {
            if( inst->hasKind(Inst::Kind_PseudoInst)) {
                
                uint8 * instStartIp = ip;
                inst->setCodeOffset( (uint32)(instStartIp-blockStartIp) );
                continue;
            }

            if (inst->hasKind(Inst::Kind_ControlTransferInst) && 
                   ((ControlTransferInst*)inst)->isDirect() && 
                   inst->getMnemonic() == Mnemonic_CALL) 
            {
                ControlTransferInst* callInst =  (ControlTransferInst*)inst;
                Opnd::RuntimeInfo * rt = callInst->getOpnd(callInst->getTargetOpndIndex())->getRuntimeInfo();
                bool callIsNotForPatching = rt && (rt->getKind() == Opnd::RuntimeInfo::Kind_InternalHelperAddress ||
                                                   rt->getKind() == Opnd::RuntimeInfo::Kind_HelperAddress);

                if (!callIsNotForPatching) { // the call may be patched at runtime
                    // nops for self-jump <opcode + 8 bit displacement(-3)> for atomic write at aligned ip
                    // there are 3 bytes reserved to allow self-jump to be aligned for sure
                    // the first (must be the real inst in CFG to cheat code compactor that removes these nops
                    // if the inst is the only in the bb)
                    Inst* nopInst = irManager->newInst(Mnemonic_NOP);
                    bb->prependInst(nopInst,inst);
                    ip = nopInst->emit(ip);
                    // the last two
                    ip = (uint8*)EncoderBase::nops((char*)ip,2);
#ifdef _EM64T_
                    // these nops are required for call transformation from immediate into register form
                    // nops for MOV r11, callTarget (when !fit32(call_offset) ) <opcode + 8 byte address>
                    ip = (uint8*)EncoderBase::nops((char*)ip, 10);
#endif
                }
            }
            
            uint8 * instStartIp = ip;
            assert(fit32(instStartIp-blockStartIp));
            inst->setCodeOffset( (uint32)(instStartIp-blockStartIp) );
            ip = inst->emit(ip);
        }
        bb->setCodeSize( (uint32)(ip-blockStartIp) );
    }
    unsigned codeSize = (unsigned)(ip-codeStreamStart);
    assert( codeSize < maxMethodSize );

    uint8 * codeBlock = (uint8*)irManager->getCompilationInterface().allocateCodeBlock(
            codeSize , JMP_TARGET_ALIGMENT, getCodeSectionHeat(0), 0, false );
    memcpy(codeBlock, codeStreamStart, codeSize); 
    irManager->setCodeStartAddr(codeBlock);

    //^
    //|
    //+---- malloc() above
    free( codeStreamStart );
    //
}

//________________________________________________________________________________________
void CodeEmitter::packCode() {
    bool newOpndsCreated = false;
    for( BasicBlock * bb = (BasicBlock*)irManager->getFlowGraph()->getEntryNode(), * succ; bb != NULL; bb = succ) {
        succ = (BasicBlock*)bb->getLayoutSucc();
        Inst* inst = (Inst*)bb->getLastInst();
        if (inst != NULL){
            if (inst->hasKind(Inst::Kind_ControlTransferInst) && ((ControlTransferInst*)inst)->isDirect()){
                BasicBlock * bbTarget = NULL;
                if (inst->hasKind(Inst::Kind_BranchInst))
                    bbTarget=(BasicBlock*)((BranchInst*)inst)->getTrueTarget();
                else if (inst->hasKind(Inst::Kind_JmpInst))
                    bbTarget = (BasicBlock*)bb->getUnconditionalEdgeTarget();
                if (bbTarget != NULL){
                    uint8 * instCodeStartAddr = (uint8*)inst->getCodeStartAddr();
                    uint8 * instCodeEndAddr = (uint8*)instCodeStartAddr+inst->getCodeSize();
                    uint8 * targetCodeStartAddr = (uint8*)bbTarget->getCodeStartAddr();
                    uint32 targetOpndIndex = ((ControlTransferInst*)inst)->getTargetOpndIndex();
                    int64 offset=targetCodeStartAddr-instCodeEndAddr;               
                    if (offset >= -128 && offset < 127 && inst->getOpnd(targetOpndIndex)->getSize() != OpndSize_8) {
                        inst->setOpnd(targetOpndIndex, irManager->newImmOpnd(irManager->getTypeFromTag(Type::Int8), offset));
                        uint8 * newInstCodeEndAddr = inst->emit(instCodeStartAddr);
                        bb->setCodeSize(bb->getCodeSize() + (uint32)(newInstCodeEndAddr - instCodeEndAddr));
                        newOpndsCreated = true;
                    } 
                }
            }

            if (newOpndsCreated && succ != NULL){
                int bbCodeOffset = bb->getCodeOffset();
                int succCodeOffset = succ->getCodeOffset();
                int bbCodeSize = bb->getCodeSize();
                int succCodeSize = succ->getCodeSize();
                int bbDisplacement = succCodeOffset - bbCodeOffset - bbCodeSize;
                if (bbDisplacement != 0){
                    uint8 * ps = (uint8*)irManager->getCodeStartAddr() + succCodeOffset;
                    uint8 * pd = ps - bbDisplacement;
                    uint8 * pn = ps + succCodeSize; 
                    while (ps < pn)
                        *pd++ = *ps++;
                    succ->setCodeOffset(succCodeOffset - bbDisplacement);
                }
            } 
        }
    }
    if (newOpndsCreated)
        irManager->fixLivenessInfo();
}

//________________________________________________________________________________________
void CodeEmitter::postPass()
{  
    MemoryManager& irmm = irManager->getMemoryManager();
    bool isBcRequired = irManager->getCompilationInterface().isBCMapInfoRequired();
    BcMap* bcMap = new(irmm) BcMap(irmm);
    irManager->setInfo("bcMap", bcMap);
    bool newOpndsCreated = false;
    for( BasicBlock * bb = (BasicBlock*)irManager->getFlowGraph()->getEntryNode(); bb != NULL; bb=bb->getLayoutSucc()) {
        for (Inst* inst = (Inst*)bb->getFirstInst(); inst!=NULL; inst = inst->getNextInst()) {
            if (inst->hasKind(Inst::Kind_ControlTransferInst) && 
                ((ControlTransferInst*)inst)->isDirect()
            ){
                uint8 * instCodeStartAddr=(uint8*)inst->getCodeStartAddr();
                uint8 * instCodeEndAddr=(uint8*)instCodeStartAddr+inst->getCodeSize();
                uint8 * targetCodeStartAddr=0;
                uint32 targetOpndIndex = ((ControlTransferInst*)inst)->getTargetOpndIndex();
                MethodDesc * md = NULL;
                if (inst->hasKind(Inst::Kind_BranchInst)){
                    BasicBlock * bbTarget=(BasicBlock*)((BranchInst*)inst)->getTrueTarget();
                    targetCodeStartAddr=(uint8*)bbTarget->getCodeStartAddr();
                } else if (inst->hasKind(Inst::Kind_JmpInst)){
                    BasicBlock* bbTarget = (BasicBlock*)bb->getUnconditionalEdgeTarget();
                    targetCodeStartAddr=(uint8*)bbTarget->getCodeStartAddr();
                } else if (inst->hasKind(Inst::Kind_CallInst)){
                    Opnd * targetOpnd=inst->getOpnd(targetOpndIndex);
                    targetCodeStartAddr=(uint8*)(POINTER_SIZE_INT)targetOpnd->getImmValue();
                    Opnd::RuntimeInfo * ri=targetOpnd->getRuntimeInfo();
                    if ( ri && ri->getKind() == Opnd::RuntimeInfo::Kind_MethodDirectAddr ) {
                        md = (MethodDesc*)ri->getValue(0);
                    }
                } else 
                    continue;
                int64 offset=targetCodeStartAddr-instCodeEndAddr;

                uint64 bcOffset = isBcRequired ? bc2LIRMapHandler->getVectorEntry(inst->getId()) : ILLEGAL_VALUE;
#ifdef _EM64T_
                if ( !fit32(offset) ) { // offset is not a signed value that fits into 32 bits
                    // this is for direct calls only
                    assert(inst->hasKind(Inst::Kind_CallInst));
                    Type* targetType = irManager->getTypeManager().getInt64Type();

                    Opnd* targetVal = irManager->newImmOpnd(targetType,(int64)targetCodeStartAddr);
                    Opnd* targetReg = irManager->newRegOpnd(targetType, RegName_R11);
                    
                    Inst* movInst = irManager->newInst(Mnemonic_MOV, targetReg, targetVal);

                    inst->setOpnd(targetOpndIndex,targetReg);

                    bb->prependInst(movInst,inst);

                    uint8* blockStartIp = (uint8*)bb->getCodeStartAddr();
                    // 10 bytes are dumped with 'nops' ahead of the call especially for this MOV
                    uint8* movAddr = instCodeStartAddr-10;
                    movInst->setCodeOffset((uint32)(movAddr - blockStartIp));
                    uint8* callAddr = movInst->emit(movAddr);
                    assert(callAddr == instCodeStartAddr);
                    // then dump 2 bytes with nops to keep return ip the same for both
                    // immediate and register calls
                    EncoderBase::nops((char*)(callAddr), 2);
                    // reemit the call as a register-based at the address 'callAddr+2'
                    inst->emit(callAddr+2);
                    inst->setCodeOffset( (uint32)(callAddr + 2 - blockStartIp) );
                    // the register call is being registered for patching in the same way as immediate calls
                    // code patcher takes care of the correct patching
                    if(md) {
                        // the call was generated at callAddr+2 but it is being reported as it is at callAddr
                        // code patcher knows about this collision
                        registerDirectCall(md,callAddr);
                    }

                    if (bcOffset != ILLEGAL_VALUE) {
                        bcMap->setEntry((uint64)(POINTER_SIZE_INT)movAddr, bcOffset); // MOV
                        // add both possible calls into bcMap
                        bcMap->setEntry((uint64)(POINTER_SIZE_INT)callAddr, bcOffset); // CALL (immediate)
                        bcMap->setEntry((uint64)(POINTER_SIZE_INT)callAddr+2, bcOffset); // CALL (register)
                    }

                    newOpndsCreated = true;
                } 
                else 
#endif
                {
                    inst->getOpnd(targetOpndIndex)->assignImmValue(offset);
                    // re-emit the instruction: 
                    inst->emit(instCodeStartAddr);

                    if (inst->hasKind(Inst::Kind_CallInst) && md){
                        registerDirectCall(md,instCodeStartAddr);
                    }

                    if (bcOffset != ILLEGAL_VALUE) {
                        bcMap->setEntry((uint64)(POINTER_SIZE_INT)instCodeStartAddr, bcOffset);
                        if (inst->hasKind(Inst::Kind_CallInst)){
                            // the call can be moved two bytes further after transformation
                            // into register form during code patching 
                            bcMap->setEntry((uint64)(POINTER_SIZE_INT)instCodeStartAddr+2, bcOffset);
                        }
                    }
                }
            }   
        }
    }  
    if (newOpndsCreated)
        irManager->fixLivenessInfo();
}

//________________________________________________________________________________________
void CodeEmitter::registerDirectCall(MethodDesc * md, void * instStartAddr)
{
    irManager->getCompilationInterface().setNotifyWhenMethodIsRecompiled(md,instStartAddr);
    if (Log::isEnabled()) {
        Log::out() << "Registered call to " << md->getParentType()->getName() << "." << md->getName() << " at ";
        Log::out() << instStartAddr << " for recompiled method event" << ::std::endl;
    }
}

//________________________________________________________________________________________
bool RuntimeInterface::recompiledMethodEvent(MethodDesc *recompiledMethodDesc, 
                           void *                    data) 
{
    Byte ** indirectAddr = (Byte **)recompiledMethodDesc->getIndirectAddress();
    Byte * targetAddr = *indirectAddr;
    Byte * callAddr = (Byte*)data;

    uint64 offset = targetAddr - callAddr - 5;

    // we can not guarantee the (callAddr+1) aligned
    // self-jump is a kind of lock for the time of call patching
    uint32 movSize =
#ifdef _EM64T_
                     10;
#else
                     0; // no additional MOV on ia32
#endif
    Byte * movAddr = callAddr - movSize;

    // there is a reserved region for this self-jump dumped with nops
    // it is from 'callAddr - movSize' (movAddr)
    //                   [- 10 (reserved for 'MOV r11, callTarget' if !fit32(offset))] (EM64T only)
    //                   - 3  (to provide self-jump aligned)
    // to 'callAddr - movSize' (movAddr)
    Byte* jmpAddr = movAddr - 3;
    if( 0 != (POINTER_SIZE_INT(jmpAddr) & 0x1) ) {
        jmpAddr++;
    }
    assert(POINTER_SIZE_INT(movAddr-jmpAddr) >= 2);

    // as jmpAddr is aligned emit it

    //                   JMP opcode  | jmp disp (-2)
    uint16 jmpWithNop = (0xeb)       | (0xfe << 8);
    *(uint16*)(jmpAddr) = jmpWithNop;
    
    // do we need 'lfence' here?
    // no. it is not necessary.

    // The patching itself

#ifdef _EM64T_
    bool registerCallIsBeingPatched = ( 0xB8 == (0xF8 &(*(movAddr+1))) ); // test opcode (&0xF8 - to skip rd bits)

    EncoderBase::Operands args;
    if ( !fit32(offset) ) { // offset is not a signed value that fits into 32 bits
        // - encode mov reg, newTarget
        args.clear();
        args.add(RegName_R11);
        // direct call <imm> is relative, but call <reg> use an absolute address to jump
        args.add(EncoderBase::Operand(OpndSize_64, (int64)targetAddr));
        EncoderBase::encode((char*)movAddr, Mnemonic_MOV, args);
        if (!registerCallIsBeingPatched) {
            // dump the first 2 bytes with nops
            // to keep return ip the same
            EncoderBase::nops((char*)(callAddr), 2);
            // - replace old (immediate) call by the register-formed
            args.clear();
            args.add(RegName_R11);
            EncoderBase::encode((char*)(callAddr+2), Mnemonic_CALL, args); // 3 bytes
        }

    } else if (registerCallIsBeingPatched) { // but offset fits into 32 bits
        // replace old mov with nops
        EncoderBase::nops((char*)(movAddr), 10);
        // emit the call in immediate form
        args.clear();
        args.add(EncoderBase::Operand(OpndSize_32, (int32)offset));
        EncoderBase::encode((char*)callAddr, Mnemonic_CALL, args); // 5 bytes
    } else
#endif
    { // offset fits into 32 bits
        *(uint32*)(callAddr+1) = (uint32)offset;
    }

    // removing self-jump
    *(uint16*)(jmpAddr) = 0x9090; // 2 nops
    
    // do we need 'lfence' here?
    // no. it has some sense but it is not necessary.

    return true;
}

//________________________________________________________________________________________
CodeBlockHeat CodeEmitter::getCodeSectionHeat(uint32 sectionID)const
{
    CodeBlockHeat heat;
    if (irManager->getCompilationContext()->hasDynamicProfileToUse())
        heat = CodeBlockHeatDefault;
    else if (sectionID==0)
        heat = CodeBlockHeatMax;
    else
        heat = CodeBlockHeatMin;
    
    return heat;
}

//________________________________________________________________________________________
void CodeEmitter::registerExceptionHandlers()
{
    ControlFlowGraph* fg = irManager->getFlowGraph();
    Node* unwind = fg->getUnwindNode();
    POINTER_SIZE_INT regionStart=0, regionEnd=0; Node * regionDispatchNode=NULL;
    for( BasicBlock * bb = (BasicBlock*)fg->getEntryNode(); bb != NULL; bb=bb->getLayoutSucc()) {
        Node * dispatchNode= bb->getExceptionEdgeTarget();
        if (regionDispatchNode!=dispatchNode) {
            if (regionDispatchNode!=NULL && regionDispatchNode!=unwind && regionStart<regionEnd) {
                registerExceptionRegion((void*)regionStart, (void*)regionEnd, regionDispatchNode);
            }
            regionDispatchNode=dispatchNode;
            regionStart=regionEnd=(POINTER_SIZE_INT)bb->getCodeStartAddr();
        }
        if (dispatchNode!=NULL){
            regionEnd+=bb->getCodeSize();
        }
    }
    if (regionDispatchNode!=NULL && regionDispatchNode!=unwind && regionStart<regionEnd) {
        registerExceptionRegion((void*)regionStart, (void*)regionEnd, regionDispatchNode);
    }

    uint32 handlerInfoCount=(uint32)exceptionHandlerInfos.size();
    irManager->getMethodDesc().setNumExceptionHandler(handlerInfoCount);
    for (uint32 i=0; i<handlerInfoCount; i++){
        const ExceptionHandlerInfo & info=exceptionHandlerInfos[i];
        if (Log::isEnabled()) {
            Log::out() << "Exception Handler Info [ " << i << "]: " << ::std::endl;
            Log::out() << "    " << (void*)(info.regionStart) << " - "<<(void*)(info.regionEnd) << 
                " => " << (void*)info.handlerAddr << ::std::endl << "    ";
            IRPrinter::printType(Log::out(), info.exceptionType);
            Log::out() << ::std::endl;
        }

        irManager->getMethodDesc().
            setExceptionHandlerInfo(i,
                (Byte*)info.regionStart, (Byte*)info.regionEnd,
                (Byte*)info.handlerAddr,
                info.exceptionType,
                info.exceptionObjectIsDead);
    }

}


static bool edge_prior_comparator(const Edge* e1, const Edge* e2) {
    assert(e1->isCatchEdge() && e2->isCatchEdge());
    uint32 p1 = ((CatchEdge*)e1)->getPriority();
    uint32 p2 = ((CatchEdge*)e2)->getPriority();
    assert(p1!=p2 || e1==e2);
    return p1 < p2 ? true : p1 > p2 ? false : e1 > e2;
};

//________________________________________________________________________________________
void CodeEmitter::registerExceptionRegion(void * regionStart, void * regionEnd, Node* regionDispatchNode)
{
    assert(regionDispatchNode->isDispatchNode());
    assert(regionStart!=NULL && regionStart<regionEnd && regionDispatchNode!=NULL);
    assert(regionDispatchNode!=irManager->getFlowGraph()->getUnwindNode());

    const Edges& edges=regionDispatchNode->getOutEdges();
    Edges catchEdges(irManager->getMemoryManager());
    Node* dispatchHead = NULL;
    for (Edges::const_iterator ite = edges.begin(), ende = edges.end(); ite!=ende; ++ite) {
        Edge* edge = *ite;
        if (edge->isCatchEdge()) {
            catchEdges.push_back(edge);
        } else {
            assert(dispatchHead == NULL);
            dispatchHead = edge->getTargetNode();
            assert(dispatchHead->isDispatchNode());
        }
    }
    std::sort(catchEdges.begin(), catchEdges.end(), edge_prior_comparator);

    for (Edges::const_iterator ite = catchEdges.begin(), ende = catchEdges.end(); ite!=ende; ++ite) {
        Edge* edge = *ite;
        BasicBlock* handler = (BasicBlock*)edge->getTargetNode();
        Inst * catchInst = (Inst*)handler->getFirstInst();
        assert(catchInst && catchInst->getKind() == Inst::Kind_CatchPseudoInst);
        MemoryManager mm(0x400, "CatchOpnds");
        BitSet * ls=new(mm) BitSet(mm, irManager->getOpndCount());
        irManager->getLiveAtExit(handler, *ls);
        for (Inst* inst = (Inst*)handler->getLastInst(); inst!=catchInst; inst = inst->getPrevInst()) {
            irManager->updateLiveness(inst, *ls);
        }
        bool isExceptObjDead = !ls->getBit(catchInst->getOpnd(0)->getId());

        void * handlerAddr=handler->getCodeStartAddr();
        ObjectType * exceptionType = (ObjectType*)((CatchEdge*)edge)->getType();
        exceptionHandlerInfos.push_back(
            ExceptionHandlerInfo(regionStart, regionEnd, handlerAddr, exceptionType, isExceptObjDead)
            );
    }
    if (dispatchHead!=NULL && dispatchHead!=irManager->getFlowGraph()->getUnwindNode()) {
        registerExceptionRegion(regionStart, regionEnd, dispatchHead);
    }
}
//___________________________________________________________________
// Yet another CFG ordering to compute inliniees
//___________________________________________________________________
void CodeEmitter::orderNodesAndMarkInlinees(StlList<MethodMarkerPseudoInst*>& inlineStack, 
                                            Node* node, bool isForward) {
    assert(node!=NULL);
    assert(traversalInfo[node->getId()]==0); //node is white here
    uint32 nodeId = node->getId();
    traversalInfo[nodeId] = 1; //mark node gray
    // preprocess the node
    if (node->getKind() == Node::Kind_Block) {
        MethodMarkerPseudoInst* methMarkerInst = NULL;
        CompiledMethodInfo* methInfo = NULL;

        if (!inlineStack.empty()) {
            methMarkerInst = inlineStack.back();
            methInfo = methodLocationMap[methMarkerInst];
        }

        for (Inst* inst = (Inst*)node->getFirstInst(); inst!=NULL; inst = inst->getNextInst()) {
            if (inst->getKind() == Inst::Kind_MethodEntryPseudoInst) {
                // keep old method entry
                MethodMarkerPseudoInst* oldMethodEntryInst = methMarkerInst;
                assert(!oldMethodEntryInst || oldMethodEntryInst->getKind() == Inst::Kind_MethodEntryPseudoInst);
                // set new methMarkerInst
                methMarkerInst = (MethodMarkerPseudoInst*)inst;
                inlineStack.push_back(methMarkerInst); 
                // construct new inlined method info
                methInfo = new(memoryManager) CompiledMethodInfo(memoryManager,
                                                                 (POINTER_SIZE_INT)methMarkerInst->getCodeStartAddr(),
                                                                 oldMethodEntryInst,
                                                                 (uint32)inlineStack.size());

                methodLocationMap[methMarkerInst] = methInfo;
            } else if (inst->getKind() == Inst::Kind_MethodEndPseudoInst) {
                methMarkerInst = (MethodMarkerPseudoInst*)inst;
                assert(((MethodMarkerPseudoInst*)inlineStack.back())->getMethodDesc() == methMarkerInst->getMethodDesc());
                entryExitMap[methMarkerInst] = inlineStack.back();
                inlineStack.pop_back();
                methMarkerInst = NULL;
            } else {              //handle usual instructions
                if (methMarkerInst != NULL) {    // inlined locations for methMarkerInst 
                    assert(methInfo == methodLocationMap[methMarkerInst]);
                    if( ! inst->hasKind(Inst::Kind_PseudoInst)) {
                        instSizeMap[(POINTER_SIZE_INT) inst->getCodeStartAddr()] = inst->getCodeSize();
                    }
                    uint64 instID = inst->getId();
                    uint64 bcOffset = bc2LIRMapHandler->getVectorEntry(instID);
                    methInfo->includeInst(inst,bcOffset);
                    // addLocation with ILLEGAL_VALUE for all outers
                    MethodMarkerPseudoInst* outerEntry = methInfo->getOuterMethodEntry();
                    while (outerEntry) {
                        CompiledMethodInfo* outerInfo = methodLocationMap[outerEntry];
                        outerInfo->includeInst(inst, ILLEGAL_VALUE);
                        outerEntry = outerInfo->getOuterMethodEntry();
                    }
                }
            }
        }
    }

    const Edges& edges=node->getEdges(isForward);
    Edges::const_reverse_iterator edge_riter;

    for(edge_riter = edges.rbegin(); edge_riter != edges.rend(); ++edge_riter) {
        Edge* e = *edge_riter;
        Node* targetNode  = e->getNode(isForward);
        if ( traversalInfo[targetNode->getId()] !=0) {
            //back-edge(gray) if == 1 or cross-edge or direct-edge (black) if ==2
            continue; 
        }
        orderNodesAndMarkInlinees(inlineStack, targetNode, isForward);
    }
    traversalInfo[nodeId] = 2;//mark node black

    // postprocess the node with reverse instruction order
    if (node->getKind() == Node::Kind_Block) {
        BasicBlock* bb = (BasicBlock*)node;

        for (Inst* inst = (Inst*)bb->getLastInst(); inst != NULL; inst = inst->getPrevInst()) {
            if (inst->getKind() == Inst::Kind_MethodEndPseudoInst) {
                MethodMarkerPseudoInst* methMarkerInst = (MethodMarkerPseudoInst*)inst;
                inlineStack.push_back(entryExitMap[methMarkerInst]);
            } else if (inst->getKind() == Inst::Kind_MethodEntryPseudoInst) {
                //MethodMarkerPseudoInst* methMarkerInst = (MethodMarkerPseudoInst*)inst;
                assert(((MethodMarkerPseudoInst*)inlineStack.back())->getMethodDesc() == ((MethodMarkerPseudoInst*)inst)->getMethodDesc());
                inlineStack.pop_back();
            } 
        }
    }
    
}

void CodeEmitter::reportInlinedMethod(CompiledMethodInfo* methInfo, MethodMarkerPseudoInst* methEntryInst) {
    AddrLocation* addrLocationMap = new(memoryManager) AddrLocation[methInfo->locationMap.size()];

    MethodDesc* method = methEntryInst->getMethodDesc();
    MethodMarkerPseudoInst* outerEntry = methInfo->getOuterMethodEntry();
    MethodDesc* outer = (outerEntry != NULL) ? outerEntry->getMethodDesc() : &irManager->getMethodDesc();

    LocationMap::const_iterator lit, 
                litStart = methInfo->locationMap.begin(),
                litEnd = methInfo->locationMap.end();

    POINTER_SIZE_INT methodStartAddr = litStart == litEnd ? methInfo->getCodeAddr() : (*litStart).first;
    POINTER_SIZE_INT prevAddr = 0;
    uint32 methodSize = 0;
    uint32 locationMapSize = 0;

    for (lit = litStart; lit != litEnd; lit++) {

        POINTER_SIZE_INT addr = (*lit).first;
        uint16 bcOffset = (*lit).second;
        assert( addr > prevAddr); // locationMap content must be ordered

        if (addr != prevAddr + instSizeMap[prevAddr] && methodSize > 0) {
            // gap in layout
            assert(instSizeMap.has(prevAddr));
            assert(addr > prevAddr + instSizeMap[prevAddr]);

            // report already gathered
            irManager->getCompilationInterface().sendCompiledMethodLoadEvent(method, outer, methodSize,
                            (void*)methodStartAddr, locationMapSize, addrLocationMap, NULL);
            if (Log::isEnabled()) {
                Log::out() << "Reporting Inlinee (part)  " << method->getParentType()->getName() << "."
                                                           << method->getName() << " ";
                Log::out() << "Outer = " << outer->getParentType()->getName() << "." << outer->getName();
                Log::out() << " start=" << methodStartAddr << " size=" << methodSize << ::std::endl;
            }

            // reset values
            methodStartAddr = addr;
            addrLocationMap += locationMapSize; // shift pointer
            locationMapSize = 0;
            methodSize = 0;

        } 

        // process current location
        addrLocationMap[locationMapSize].start_addr = (void*)addr;
        addrLocationMap[locationMapSize].location = bcOffset;
        locationMapSize++;
        methodSize += instSizeMap[addr];
        prevAddr = addr;
    }
    // report final
    irManager->getCompilationInterface().sendCompiledMethodLoadEvent(method, outer, methodSize,
                    (void*)methodStartAddr, locationMapSize, addrLocationMap, NULL);
    if (Log::isEnabled()) {
        Log::out() << "Reporting Inlinee (final) " << method->getParentType()->getName() << "."
                                                   << method->getName() << " ";
        Log::out() << "Outer = " << outer->getParentType()->getName() << "." << outer->getName();
        Log::out() << " start=" << methodStartAddr << " size=" << methodSize << ::std::endl;
    }

}

void CodeEmitter::reportCompiledInlinees() {
    StlList<MethodMarkerPseudoInst*> inlineList(memoryManager);
    traversalInfo.resize(irManager->getFlowGraph()->getMaxNodeId() + 1, 0);
    bool isForward = true;
    orderNodesAndMarkInlinees(inlineList, irManager->getFlowGraph()->getEntryNode(), isForward);

    StlMap<MethodMarkerPseudoInst*, CompiledMethodInfo* >::const_iterator i, 
            itEnd = methodLocationMap.end();

    // send events according to inlining depth
    // depth == 1 - the first level inlinees
    bool found = false;
    uint32 depth = 1;
    do {
        found = false;
        for (i = methodLocationMap.begin(); i != itEnd; i++) {
            MethodMarkerPseudoInst* methEntryInst = (MethodMarkerPseudoInst*)i->first;
            CompiledMethodInfo* methInfo = (CompiledMethodInfo*)i->second;
            if (depth == methInfo->getInlineDepth()) {
                found = true;
                reportInlinedMethod(methInfo, methEntryInst);
            }
        }
        depth++;
    } while (found);
}


}}; // ~Jitrino::Ia32




