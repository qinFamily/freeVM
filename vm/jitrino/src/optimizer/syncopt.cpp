/*
 *  Copyright 2005-2006 The Apache Software Foundation or its licensors, as applicable.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
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
 * @author Intel, Pavel A. Ozhdikhin
 * @version $Revision: 1.22.24.4 $
 *
 */

#include <assert.h>
#include <iostream>
#include <algorithm>
#include "Stl.h"
#include "Log.h"
#include "PropertyTable.h"
#include "open/types.h"
#include "Inst.h"
#include "irmanager.h"
#include "FlowGraph.h"
#include "Dominator.h"
#include "Loop.h"
#include "Opcode.h"
#include "walkers.h"
#include "opndmap.h"
#include "dataflow.h"
#include "unionfind.h"
#include "syncopt.h"

namespace Jitrino {

#define EXTRA_DEBUGGING 0

DEFINE_OPTPASS_IMPL(SyncOptPass, syncopt, "Synchronization Optimization")

void
SyncOptPass::_run(IRManager& irm) {
    SyncOpt opt(irm, irm.getNestedMemoryManager());
    opt.runPass();
}

SyncOpt::Flags *SyncOpt::defaultFlags = 0;

SyncOpt::SyncOpt(IRManager &irManager0, 
                 MemoryManager& memManager)
    : irManager(irManager0),
      mm(memManager),
      flags(*defaultFlags),
      lockType(0),
      lockAddrType(0),
      lockVar(memManager)
{
    assert(defaultFlags);
}

SyncOpt::~SyncOpt()
{
}

void 
SyncOpt::readDefaultFlagsFromCommandLine(const JitrinoParameterTable *params)
{
    if (!defaultFlags)
        defaultFlags = new Flags;

    defaultFlags->optimistic = params->lookupBool("opt::sync::optimistic", false);
    defaultFlags->use_IncRecCount 
        = params->lookupBool("opt::sync::use_increccount", false);
    defaultFlags->balance = params->lookupBool("opt::sync::balance", true);
    defaultFlags->transform = params->lookupBool("opt::sync::transform", true);
    defaultFlags->transform2 = params->lookupBool("opt::sync::transform2", true);
}

void SyncOpt::showFlagsFromCommandLine()
{
    Log::out() << "    opt::sync::optimistic[={OFF|on}] = convert region with a call to optimistic balanced enter/exit" << ::std::endl;
    Log::out() << "    opt::sync::use_increccount[={OFF|on}]  = use increccount [not working yet]" << ::std::endl;
    Log::out() << "    opt::sync::balance[={off|ON}] = do balancing" << ::std::endl;
    Log::out() << "    opt::sync::transform[={off|ON}] = do rewriting that facilitates balancing" << ::std::endl;
    Log::out() << "    opt::sync::transform2[={OFF|on}] = give dispatch nodes a dispatch succ" << ::std::endl;
}


// a NodeWalker
class FixupSyncEdgesWalker {
protected:
    SyncOpt *thePass;
    IRManager &irm;
    MemoryManager &mm;
    FlowGraph &fg;
    CFGNode *unwind;
public:
    FixupSyncEdgesWalker(IRManager &irm0,
                         MemoryManager& mm0)
        : irm(irm0), mm(mm0), fg(irm0.getFlowGraph()), unwind(0)
    {
        unwind = fg.getUnwind();
    };

    void applyToCFGNode(CFGNode *node);
    bool applyToNode1(CFGNode *node);
    bool applyToNode2(CFGNode *node);
    bool applyToNode3(CFGNode *node);
    
    bool isDispatchNode(CFGNode *node) { return node->isDispatchNode(); };
    bool isCatchAll(CFGNode *node, Opnd *&caughtOpnd, CFGNode *&catchInstNode);
    bool isMonExit(CFGNode *node, Opnd *&monOpnd, CFGNode *&dispatchNode,
                   CFGEdge *&dispatchEdge);
    bool isCatchAllAndMonExit(CFGNode *node, Opnd *&caughtOpnd, Opnd *&monOpnd, CFGNode *&dispatchNode,
                              CFGEdge *&dispatchEdge,
                              CFGNode *&catchInstNode);
    bool isJustThrow(CFGNode *node, Opnd *&thrownOpnd, CFGNode *&dispatchNode);
    bool isUnwind(CFGNode *node) { return (node == unwind); };
    CFGNode *getFirstCatchNode(CFGNode *dispatchNode);
};

bool FixupSyncEdgesWalker::isCatchAll(CFGNode *node, Opnd *&caughtOpnd,
                                      CFGNode *&catchInstNode)
{
    Inst *firstInst = node->getFirstInst();
    if (!firstInst->isCatchLabel()) return false;
    CatchLabelInst *catchLabelInst = firstInst->asCatchLabelInst();
    Type *exceptionType = catchLabelInst->getExceptionType();
    if (exceptionType != irm.getTypeManager().getSystemObjectType()) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " isCatchAll case 1" << ::std::endl;
        }
        return false;
    }

    Inst *nextInst = firstInst->next();
    CFGNode *ciNode = node;
    int counter = 0;
    while (nextInst->getOpcode() != Op_Catch) {
        if (nextInst == firstInst) {
            if (++counter > 3) {
                if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
                    Log::out() << " isCatchAll case 5" << ::std::endl;
                }
                // don't think this is possible, but bound the loop just in case.
                return false;
            }
            if (!ciNode->hasOnlyOneSuccEdge()) {
                if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
                    Log::out() << " isCatchAll case 4" << ::std::endl;
                }
                return false;
            }
            CFGNode *succ = (*(ciNode->getOutEdges().begin()))->getTargetNode();
            ciNode = succ;
            firstInst = succ->getFirstInst();
            nextInst = firstInst->next();
        }
    }
    Opnd *opnd = nextInst->getDst();

    if (opnd->getType() == irm.getTypeManager().getSystemObjectType()) {
        // yes
        catchInstNode = ciNode;
        caughtOpnd = opnd;
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " isCatchAll case 2" << ::std::endl;
        }
        return true;
    }
    if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << " isCatchAll case 3" << ::std::endl;
    }
    return false;
}

bool FixupSyncEdgesWalker::isMonExit(CFGNode *node, Opnd *&monOpnd, CFGNode *&dispatchNode,
                                     CFGEdge *&dispatchEdge)
{
    Inst *lastInst = node->getLastInst();
    Opcode opcode = lastInst->getOpcode();
    if (!((opcode == Op_TauMonitorExit) ||
          (opcode == Op_OptimisticBalancedMonitorExit))) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "isMonExit opcode is " << (int) opcode << ::std::endl;
        }
        return false;
    }

    monOpnd = lastInst->getSrc(0);
    const CFGEdgeDeque &outEdges = node->getOutEdges();
    CFGEdgeDeque::const_iterator
        eiter = outEdges.begin(),
        eend = outEdges.end();
    for ( ; eiter != eend; ++eiter) {
        CFGEdge *e = *eiter;
        CFGNode *target = e->getTargetNode();
        if (target->isDispatchNode()) {
            dispatchNode = target;
            dispatchEdge = e;
            return true;
        }
    }
    assert(0);
    return false;
}

bool FixupSyncEdgesWalker::isCatchAllAndMonExit(CFGNode *node, Opnd *&caughtOpnd,
                                                Opnd *&monOpnd, CFGNode *&dispatchNode,
                                                CFGEdge *&dispatchEdge,
                                                CFGNode *&catchInstNode)
{
    catchInstNode = 0;
    if (isCatchAll(node, caughtOpnd, catchInstNode) &&
        isMonExit(catchInstNode, monOpnd, dispatchNode, dispatchEdge)) {
        // make sure that's all it is
        assert(catchInstNode);
        Inst *labelInst = catchInstNode->getFirstInst();
        Inst *catchInst = labelInst->next();
        // with DPGO, we may see an incCounter here
        Inst *thirdInst = catchInst->next(); // monexit or tausafe or incCounter
        Inst *fourthInst = thirdInst->next(); // label or monexit or tausafe or incCounter
        Inst *fifthInst = fourthInst->next(); // label or taumonexit(tausafe)
        Inst *sixthInst = fifthInst->next(); // .. or label
        return ((labelInst == fourthInst)
                || ((labelInst == fifthInst) && 
                    ((thirdInst->getOpcode() == Op_TauSafe) ||
                     (thirdInst->getOpcode() == Op_IncCounter))) // fourthInst is monExit
                || ((labelInst == sixthInst) && 
                    ((thirdInst->getOpcode() == Op_TauSafe) ||
                     (thirdInst->getOpcode() == Op_IncCounter)) &&
                    ((fourthInst->getOpcode() == Op_TauSafe) ||
                     (fourthInst->getOpcode() == Op_IncCounter))) // fifthInst is monExit
                );
    }
    return false;
}

bool FixupSyncEdgesWalker::isJustThrow(CFGNode *node, Opnd *&thrownOpnd, CFGNode *&dispatchNode)
{
    Inst *lastInst = node->getLastInst();
    Opcode opcode = lastInst->getOpcode();
    if (opcode != Op_Throw) {
        return false;
    }
    Inst *prevInst = lastInst->prev();
    if (prevInst->isLabel()) {
        // check for dispatchNode
        assert(node->hasOnlyOneSuccEdge());
        const CFGEdgeDeque &outEdges = node->getOutEdges();
        assert(outEdges.size() == 1);
        CFGEdge *e = *(outEdges.begin());
        CFGNode *target = e->getTargetNode();
        assert((target == unwind) || (target->isDispatchNode()));

        thrownOpnd = lastInst->getSrc(0);
        dispatchNode = target;
        return true;
    }
    return false;
}

CFGNode *FixupSyncEdgesWalker::getFirstCatchNode(CFGNode *dispatchNode)
{
    assert(dispatchNode->isDispatchNode());
    const CFGEdgeDeque &outEdges = dispatchNode->getOutEdges();
    CFGEdgeDeque::const_iterator
        eiter = outEdges.begin(),
        eend = outEdges.end();
    for ( ; eiter != eend; ++eiter) {
        CFGEdge *e = *eiter;
        CFGNode *target = e->getTargetNode();
        if (!target->isDispatchNode()) {
            return target;
        }
    }
    assert(0);
    return false;
}

// look for a particular idiom:
//    monexit t1 --> Dispatch1 --> Catch(t2:object); monexit t1 --> throw t2 --> Dispatch2
// and convert it to
//    monexit t1 --> Dispatch2
// unless Dispatch2 == Dispatch1, in which case remove the second monexit and add a self-loop
//    on the throw node


bool 
FixupSyncEdgesWalker::applyToNode1(CFGNode *node)
{
    if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "applyToNode1 examining node"; node->print(Log::out()); Log::out() << ::std::endl;
    }

    CFGNode *dispatch1 = 0;
    CFGEdge *dispatch1edge = 0;
    Opnd *monOpnd1 = 0;
    if (!isMonExit(node, monOpnd1, dispatch1, dispatch1edge)) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 1" << ::std::endl;
        }
        return false;
    }
    if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "    case 1b" << ::std::endl;
    }

    CFGNode *catchNode = getFirstCatchNode(dispatch1);
    Opnd *caughtOpnd = 0;
    Opnd *monOpnd2 = 0;
    CFGNode *dispatchNode2ignore = 0;
    CFGEdge *dispatchEdge2ignore = 0;
    CFGNode *catchInstNode = 0;
    if (!isCatchAllAndMonExit(catchNode, caughtOpnd, monOpnd2, 
                              dispatchNode2ignore,
                              dispatchEdge2ignore,
                              catchInstNode)) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 2" << ::std::endl;
        }
        return false;
    }

    CFGEdge *outEdge = (CFGEdge *)catchInstNode->getUnconditionalEdge();
    assert(outEdge);

    CFGNode *nextNode = outEdge->getTargetNode();
    Opnd *thrownOpnd = 0;
    CFGNode *dispatch2 = 0;
    if (!isJustThrow(nextNode, thrownOpnd, dispatch2)) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 3, nextNode is ";
            nextNode->print(Log::out());
            Log::out() << ::std::endl;
        }
        return false;
    }

    if (!((monOpnd1 == monOpnd2) && (caughtOpnd == thrownOpnd))) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 4" << ::std::endl;
        }
        return false;
    }

    // found it
    if (dispatch1 == dispatch2) {
        // it's a self-loop
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 5" << ::std::endl;
        }
        assert(0);
    }

    if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "    case 6" << ::std::endl;
    }
    fg.replaceEdgeTarget(dispatch1edge, dispatch2);
    return true;
}

// check for  Dispatch -> catchall, eliminate any exception edge on the dispatch
bool
FixupSyncEdgesWalker::applyToNode2(CFGNode *node)
{
    if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "applyToNode2 examining node"; node->print(Log::out()); Log::out() << ::std::endl;
    }

    if (node == unwind) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 1" << ::std::endl;
        }
        return false;
    }
    if (!node->isDispatchNode()) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 2" << ::std::endl;
        }
        return false;
    }

    const CFGEdgeDeque &outEdges = node->getOutEdges();
    CFGEdge *dispatchEdge = 0;

    CFGEdgeDeque::const_iterator
        eiter = outEdges.begin(),
        eend = outEdges.end();
    for ( ; eiter != eend; ++eiter) {
        CFGEdge *e = *eiter;
        CFGNode *target = e->getTargetNode();
        if (target->isDispatchNode()) {
            assert(!dispatchEdge);
            dispatchEdge = e;
        }
    }

    if (!dispatchEdge) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 3" << ::std::endl;
        }
        return false;
    }

    CFGEdge *catchEdge = 0;
    for (eiter = outEdges.begin() ; eiter != eend; ++eiter) {
        CFGEdge *e = *eiter;
        if (e->getKind() == CFGEdge::Catch) {
            catchEdge = e;

            CFGNode *catchNode = catchEdge->getTargetNode();
            Opnd *scratch;
            CFGNode *catchInstNode;
            if (!isCatchAll(catchNode, scratch, catchInstNode)) {
                continue;
            }
            
            // dispatch node has a catch-all, doesn't need fall-through
            fg.removeEdge(dispatchEdge);
            if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << "    case 6" << ::std::endl;
            }
            return true;
        }
    }

    if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "    case 4" << ::std::endl;
    }
    return false;
}

// look for a particular idiom:
//    L1:Catch(t2:object); monexit t1 -->(1) Dispatch1 --> L1
// and convert it to
//    monexit t1 --> Dispatch2 --> Catch(t2:object); --> L1': goto L1'

bool 
FixupSyncEdgesWalker::applyToNode3(CFGNode *node)
{

    if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "applyToNode3 examining node"; node->print(Log::out()); Log::out() << ::std::endl;
    }

    CFGNode *catchNode = node;
    Opnd *caughtOpnd = 0;
    Opnd *monOpnd2 = 0;
    CFGNode *dispatchNode1 = 0;
    CFGEdge *dispatchEdge1 = 0;
    CFGNode *catchInstNode = 0;
    if (!isCatchAllAndMonExit(catchNode, caughtOpnd, monOpnd2, 
                              dispatchNode1,
                              dispatchEdge1,
                              catchInstNode)) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 1" << ::std::endl;
        }
        return false;
    }
    
    Inst *catchNodeFirstInst = catchNode->getFirstInst();
    CatchLabelInst *catchLabelInst = catchNodeFirstInst->asCatchLabelInst();
    assert(catchLabelInst);

    if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "    case 2" << ::std::endl;
    }

    // must be high-priority catch node
    uint32 order = catchLabelInst->getOrder();
    if (order != 0) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 3" << ::std::endl;
        }
        return false;
    }

    assert(dispatchNode1 && (dispatchNode1->isDispatchNode()));

    // must be only exception edge to this dispatch
    if (dispatchNode1->getInEdges().size() != 1) {
        if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "    case 4" << ::std::endl;
        }
        return false;
    }

    // find the catch node edge
    const CFGEdgeDeque &outEdges = dispatchNode1->getOutEdges();
    CFGEdgeDeque::const_iterator
        eiter = outEdges.begin(),
        eend = outEdges.end();
    for ( ; eiter != eend; ++eiter) {
        CFGEdge *e = *eiter;
        CFGNode *target = e->getTargetNode();
        if (target == catchNode) {

            if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << "    case 5" << ::std::endl;
            }

            TypeManager &typeManager = irm.getTypeManager();
            InstFactory &instFactory = irm.getInstFactory();
            OpndManager &opndManager = irm.getOpndManager();
            Opnd* ex = 
                opndManager.createSsaTmpOpnd(typeManager.getSystemObjectType());
            CFGNode *newCatchNode = fg.createCatchNode(0, ex->getType());
            newCatchNode->append(instFactory.makeCatch(ex));

            CFGNode *newLoopNode = fg.createBlockNode();
#ifdef _DEBUG
            Inst *newLoopFirstInst = newLoopNode->getFirstInst();
            assert(newLoopFirstInst);
            LabelInst *newLoopLabelInst = newLoopFirstInst->asLabelInst();
#endif
			assert(newLoopLabelInst);

            fg.replaceEdgeTarget(e, newCatchNode);
            fg.addEdge(newCatchNode, newLoopNode);
            fg.addEdge(newLoopNode, newLoopNode);
            return true;
        }
    }
    if (EXTRA_DEBUGGING && Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "    case 6" << ::std::endl;
    }
    return false;
}

void
FixupSyncEdgesWalker::applyToCFGNode(CFGNode *node)
{
    applyToNode1(node);
    applyToNode2(node);
    applyToNode3(node);
}

class FixupSyncEdgesWalker2 : public FixupSyncEdgesWalker {
public:
    FixupSyncEdgesWalker2(IRManager &irm0,
                          MemoryManager& mm0)
        : FixupSyncEdgesWalker(irm0, mm0)
    {
    };
    ~FixupSyncEdgesWalker2() {};
    void applyToCFGNode(CFGNode *node);
};

void FixupSyncEdgesWalker2::applyToCFGNode(CFGNode *node)
{
    if (node->isDispatchNode() && (node != unwind)) {
        const CFGEdgeDeque &outEdges = node->getOutEdges();
        CFGEdgeDeque::const_iterator
            eiter = outEdges.begin(),
            eend = outEdges.end();
        for ( ; eiter != eend; ++eiter) {
            CFGEdge *e = *eiter;
            CFGNode *target = e->getTargetNode();
            if (target->isDispatchNode()) {
                return;
            }
        }
        // add an edge to unwind
        
        fg.addEdge(node, unwind);
    }
}

void SyncOpt::runPass()
{
    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "Starting SyncOpt Pass" << ::std::endl;
        Log::out() << "  MemManager bytes_allocated= "
                   << (int) mm.bytes_allocated() << ::std::endl;
    }

    {
        const CFGNodeDeque &nodes =  irManager.getFlowGraph().getNodes();
        CFGNodeDeque::const_iterator 
            iter = nodes.begin(),
            end = nodes.end();
        for (; iter != end; ++iter) {
            CFGNode *node = *iter;
            Inst *firstInst = node->getFirstInst();
            Inst *inst = firstInst->next();
            while (inst != firstInst) {
                Opcode opcode = inst->getOpcode();
                switch (opcode) {
                case Op_TauMonitorEnter:
                case Op_TauMonitorExit:
                case Op_TypeMonitorEnter:
                case Op_TypeMonitorExit:
                case Op_TauOptimisticBalancedMonitorEnter:
                case Op_OptimisticBalancedMonitorExit:
                    goto found;
                default:
                    break;
                }
                inst = inst->next();
            }
        }
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "No sync found, skipping SyncOpt Pass" << ::std::endl;
            Log::out() << "  MemManager bytes_allocated= "
                       << (int) mm.bytes_allocated() << ::std::endl;
        }
        return;
    found:
        ;
    }

    if (flags.transform) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "Preprocessing graph to reduce monExit->monExit loops" << ::std::endl;
            Log::out() << "PRINTING LOG: IR before sync exception edge fixup" << ::std::endl;
            irManager.getFlowGraph().printInsts(Log::out(), irManager.getMethodDesc());
        }
        
        FixupSyncEdgesWalker fixupEdges(irManager, mm);
        NodeWalk<FixupSyncEdgesWalker>(irManager.getFlowGraph(), fixupEdges);
        
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "PRINTING LOG: IR after sync exception edge fixup" << ::std::endl;
            irManager.getFlowGraph().printInsts(Log::out(), irManager.getMethodDesc());
        }
    }

    uint32 lockWidth = 2;
    
    CompilationInterface &compIntfc = irManager.getCompilationInterface();
    CompilationInterface::ObjectSynchronizationInfo syncInfo;
    if (compIntfc.mayInlineObjectSynchronization(syncInfo)) 
        lockWidth = syncInfo.lockOwnerWidth;

    CompilationInterface::ObjectSynchronizationInfo syncInfoFromVM;
    bool mayInlineSync = irManager.getCompilationInterface().mayInlineObjectSynchronization(syncInfoFromVM);
    if (flags.balance && mayInlineSync)
    {
        TypeManager &typeManager = irManager.getTypeManager();
        // should be in the if above, but for debugging on IA32, do it anyway
        switch (lockWidth) {
        case 1:
            lockType = typeManager.getInt8Type(); break;
        case 2:
            lockType = typeManager.getInt16Type(); break;
        case 4:
            lockType = typeManager.getInt32Type(); break;
        case 8:
            lockType = typeManager.getInt64Type(); break;
        default:
            assert(0);
        }
        lockAddrType = typeManager.getManagedPtrType(lockType);

        // For each MonitorExit(x) which is preceded on every path by a MonitorEnter(x) without 
        // an intervening call, replace it by BalancedMonitorExit() and all preceding MonitorEnters by
        // BalancedMonitorEnter().  On any path leading from a BalancedMonitorEnter without a 
        // following BalancedMonitorExit, insert an IncRecCount()

        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "PRINTING LOG: IR before sync pass 1" << ::std::endl;
            irManager.getFlowGraph().printInsts(Log::out(), irManager.getMethodDesc());
        }

        findBalancedExits(false, false); // not optimistic, no increccount

        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "PRINTING LOG: IR after sync pass 1" << ::std::endl;
            irManager.getFlowGraph().printInsts(Log::out(), irManager.getMethodDesc());
        }

        if (flags.optimistic) {
            Opnd *syncMethodOpnd = 0;
            CFGNode *tmpDispatchNode = 0;
            CFGNode *tmpCatchNode = 0;
            CFGNode *tmpRethrowNode = 0;

            MethodDesc &desc = irManager.getMethodDesc();
            if (desc.isSynchronized() && !desc.isStatic()) {
                // synchronized on first param
                FlowGraph &fg = irManager.getFlowGraph();
                CFGNode *entry = fg.getEntry();
                Inst *firstInst = entry->getFirstInst();
                Inst *inst = firstInst->next();
                while (inst != firstInst) {
                    if (inst->getOpcode() == Op_DefArg) {
                        break;
                    }
                    inst = inst->next();
                }
                assert((inst != firstInst) && (inst->getOpcode() == Op_DefArg));
                Opnd *thisOpnd = inst->getDst();
                
                assert(!syncMethodOpnd);
                syncMethodOpnd = thisOpnd;
            } else {
            }

            bool needToPatchUnwind = syncMethodOpnd && (irManager.getFlowGraph().getUnwind() != NULL);
            if (needToPatchUnwind) {
                insertUnwindMonitorExit(syncMethodOpnd, tmpDispatchNode, tmpCatchNode,
                                        tmpRethrowNode);
            }
            findBalancedExits(true, false); // optimistic, no increccount

            if (needToPatchUnwind) {
                removeUnwindMonitorExit(syncMethodOpnd, tmpDispatchNode, tmpCatchNode,
                                        tmpRethrowNode);
            }
            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << "PRINTING LOG: IR after sync pass 2" << ::std::endl;
                irManager.getFlowGraph().printInsts(Log::out(), irManager.getMethodDesc());
            }
        }
    }

    if (flags.transform2) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "Postrocessing graph to add dispatch edges" << ::std::endl;
            Log::out() << "PRINTING LOG: IR before dispatch edge fixup" << ::std::endl;
            irManager.getFlowGraph().printInsts(Log::out(), irManager.getMethodDesc());
        }
        
        FixupSyncEdgesWalker2 fixupEdges(irManager, mm);
        NodeWalk<FixupSyncEdgesWalker2>(irManager.getFlowGraph(), fixupEdges);
        
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "PRINTING LOG: IR after dispatch edge fixup" << ::std::endl;
            irManager.getFlowGraph().printInsts(Log::out(), irManager.getMethodDesc());
        }
    }
    
    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "Finished SyncOpt Pass" << ::std::endl;
        Log::out() << "  MemManager bytes_allocated= "
                   << (int) mm.bytes_allocated() << ::std::endl;
    }
}

class SyncClique : public UnionFind {
private:
    SyncClique(const SyncClique &o) { assert(0); };
    void operator= (const SyncClique &o) { assert(0); };
public:
    Opnd *opnd;
    VarOpnd *oldValueVar;
    uint32 id;
    static uint32 counter;

    SyncClique() : UnionFind(), opnd(0), oldValueVar(0), id(++counter) {};
    SyncClique(Opnd *opnd0) : UnionFind(), opnd(opnd0), oldValueVar(0), id(++counter) {};
    void link(SyncClique *other) {
        UnionFind::link(other);
    };
    SyncClique *find() {
        return (SyncClique *) UnionFind::find();
    };
    void print(::std::ostream &os) {
        os << "clique" << (int) id;
        if (opnd) { 
            os << "[";
            opnd->print(os);
            os << "]";
        }
    };
};

uint32 SyncClique::counter = 0;
#define SYNC_STACK_TOP_DEPTH 20

class SyncOptDfValue {
    enum State { Top, Normal, Bottom } state;
    uint32 depth; 
public:
    uint32 getDepth() { return depth; };
    SyncOptDfValue()
        : state(Top), depth(SYNC_STACK_TOP_DEPTH) { };
    SyncOptDfValue(int)
        : state(Bottom), depth(0) { };
    void init(MemoryManager* mm) { }
    void print(::std::ostream &os) {
        switch (state) {
        case Top:
            os << "Top"; break;
        case Normal:
            os << "[" << (int) depth << "]"; break;
        case Bottom:
            os << "Bottom"; break;
        };
    }
    bool isTop() const { return (state == Top); };
    bool isBottom() const { return (state == Bottom); }
    // returns true when changed
    bool meetWith(const SyncOptDfValue &other) { 
        if (other.isTop() || isBottom()) {
            return false;
        } if (isTop() || other.isBottom()) {
            *this = other;
            return true;
        } else {
            assert((state == Normal) && (other.state == Normal));
            if (depth <= other.depth) {
                return false;
            } else {
                depth = other.depth;
                return true;
            }
        }
    };
    void invalidateMonitors(bool optimistic, bool use_IncRecCount) { 
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " before invalidateMonitors(): ";
            print(Log::out());
            Log::out() << ::std::endl;
        }
        if (optimistic) {
            // no effect;
        } else {
            state = Bottom; depth = 0;
        }
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " after invalidateMonitors(): ";
            print(Log::out());
            Log::out() << ::std::endl;
        }
    };
    void openMonitor(Inst *monitorEnter, Opnd *obj) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " before openMonitor(" 
                       << (int) monitorEnter->getId()
                       << "): ";
            print(Log::out());
            Log::out() << ::std::endl;
        }
        if (isBottom()) {
            state = Normal;
            depth = 1;
        } else if (isTop()) {
            // no change;
        } else {
            assert(state==Normal);
            if (depth < SYNC_STACK_TOP_DEPTH) {
                depth += 1;
            } else {
                state = Bottom;
                depth = 0;
            }
        }
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " after openMonitor(" 
                       << (int) monitorEnter->getId()
                       << "): ";
            print(Log::out());
            Log::out() << ::std::endl;
        }
    }
    void closeMonitor(Inst *monitorExit, Opnd *obj) { 
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " before closeMonitor(";
            monitorExit->print(Log::out());
            Log::out() << ", ";
            obj->print(Log::out());
            Log::out() << "): ";
            print(Log::out());
            Log::out() << ::std::endl;
        }
        if (state == Normal) {
            depth -= 1;
            if (depth == 0) {
                state = Bottom;
            }
        }
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " after closeMonitor(";
            obj->print(Log::out());
            Log::out() << "): ";
            print(Log::out());
            Log::out() << ::std::endl;
        }
    };
    // applies given inst to this value
    void apply(Inst *i, MemoryManager &mm, bool optimistic,
               bool use_IncRecCount) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "SyncOptDfValue visiting instr "; 
            i->print(Log::out()); 
            Log::out() << ::std::endl;
        }
        switch (i->getOpcode()) {
        case Op_TauBalancedMonitorEnter:
        case Op_TauOptimisticBalancedMonitorEnter:
            if (optimistic) break; // we can ignore this if optimistic
        case Op_TauMonitorEnter:
            {
                Opnd *obj = i->getSrc(0);
                openMonitor(i, obj);
            }; break;
        case Op_OptimisticBalancedMonitorExit:
        case Op_BalancedMonitorExit:
            if (optimistic) break; // we can ignore this if optimistic
        case Op_TauMonitorExit:
            {
                Opnd *obj = i->getSrc(0);
                closeMonitor(i, obj);
            } break;
        case Op_DirectCall:
        case Op_TauVirtualCall:
        case Op_IndirectCall:
        case Op_IndirectMemoryCall:
        case Op_JSR:
            invalidateMonitors(optimistic, use_IncRecCount);
        default:
            break;
        }
    }
};

class SyncOptTfValue : public DataflowTF<SyncOptDfValue> {
    MemoryManager &mem;
    CFGNode *node;
    bool optimistic;
    bool use_IncRecCount;
public:
    SyncOptTfValue(MemoryManager &mm, CFGNode *theNode, bool optimistic0,
                   bool use_IncRecCount0)
        : mem(mm), node(theNode), 
          optimistic(optimistic0), 
          use_IncRecCount(use_IncRecCount0) {};
    // returns true if changed
    bool apply(const SyncOptDfValue &in, SyncOptDfValue &out) { 
        SyncOptDfValue res = in;
        Inst* firstInst = node->getFirstInst();
        Inst * inst = firstInst;
        do {
            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << "visiting instr "; inst->print(Log::out()); 
                Log::out() << ::std::endl;
            }
            res.apply(inst, mem, optimistic, use_IncRecCount);
            inst = inst->next();
        } while (inst != firstInst);
        
        return out.meetWith(res);
    }
};

class SyncOptForwardInstance : public DataflowInstance<SyncOptDfValue> {
    MemoryManager &mm;
    bool optimistic;
    bool use_IncRecCount;
public:
    SyncOptForwardInstance(MemoryManager &mm0, bool optimistic0, bool use_IncRecCount0)
        : mm(mm0), optimistic(optimistic0), use_IncRecCount(use_IncRecCount0)
    {};

    typedef SyncOptDfValue ValueType;
    DataflowTF<SyncOptDfValue> *getNodeBehavior(CFGNode *node) {
        SyncOptTfValue *res = new (mm) SyncOptTfValue(mm, node, 
                                                      optimistic, use_IncRecCount);
        return res;
    }
    SyncOptDfValue getEntryValue() {
        return SyncOptDfValue(3); // bottom
    };
};

// an InstWalker
class BuildSyncCliquesWalker {
    friend class SyncOpt;
    MemoryManager &mem;

    StlVector<SyncClique *> *currentStack;
    uint32 currentDepth;
    SyncClique *bottomClique;

    StlMap<Inst *, SyncClique *> &monitorCliques;
    bool optimistic;
    bool use_IncRecCount;
    StlMap<Inst *, StlVectorSet<SyncClique *> *> &needRecCount;
public:
    StlVector<SyncClique *> *getStack() { return currentStack; };
    uint32 getDepth() { return currentDepth; };
    BuildSyncCliquesWalker(MemoryManager &mm,
                           SyncClique *bottomClique0,
                           StlMap<Inst *, SyncClique *> &monitorCliques0,
                           bool optimistic0,
                           bool use_IncRecCount0,
                           StlMap<Inst *, StlVectorSet<SyncClique *> *> &needRecCount0)
        : mem(mm),
          currentStack(0), currentDepth(0),
          bottomClique(bottomClique0),
          monitorCliques(monitorCliques0),
          optimistic(optimistic0),
          use_IncRecCount(use_IncRecCount0),
          needRecCount(needRecCount0)
    {};
    void startNode(StlVector<SyncClique *> *entryStack,
                   uint32 entryDepth)
    {
        currentStack = entryStack;
        currentDepth = entryDepth;
    };
    SyncClique *newMonitor(Opnd *obj) {
        return new (mem) SyncClique(obj);
    };
    void applyToInst(Inst *i) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "SyncOptDfValue visiting instr "; 
            i->print(Log::out()); 
            Log::out() << ::std::endl;
        }
        switch (i->getOpcode()) {
        case Op_TauBalancedMonitorEnter:
        case Op_TauOptimisticBalancedMonitorEnter:
            if (optimistic) break; // we can ignore this if optimistic
        case Op_TauMonitorEnter:
            {
                Opnd *obj = i->getSrc(0);
                openMonitor(i, obj);
            }; break;
        case Op_OptimisticBalancedMonitorExit:
        case Op_BalancedMonitorExit:
            if (optimistic) break; // we can ignore this if optimistic
        case Op_TauMonitorExit:
            {
                Opnd *obj = i->getSrc(0);
                closeMonitor(i, obj);
            } break;
        case Op_DirectCall:
        case Op_TauVirtualCall:
        case Op_IndirectCall:
        case Op_IndirectMemoryCall:
        case Op_JSR:
            invalidateMonitors(i);
            break;
        default:
            break;
        }
    }
    void invalidateMonitors(Inst *i) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " before invalidateMonitors("
                       << (int) i->getId()
                       << "), depth="
                       << (int) currentDepth
                       << ::std::endl;
        }
        if (!optimistic) {
            if (currentDepth != 0) {
                assert(currentStack);
                if (use_IncRecCount) {
                    StlVectorSet<SyncClique *> *openSet = new (mem) StlVectorSet<SyncClique *>(mem);
                    for (uint32 j=0; j < currentDepth; ++j) {
                        openSet->insert((*currentStack)[j]);
                    }
                    needRecCount[i] = openSet;
                    // leaving a balanced area, cancel all monitors.
                    currentDepth = 0;
                } else {
                    for (uint32 i=0; i<currentDepth; ++i) {
                        // invalidate all open monitors
                        if (Log::cat_opt_sync()->isDebugEnabled()) {
                            Log::out() << " linking01 ";
                            (*currentStack)[i]->print(Log::out());
                            Log::out() << " to ";
                            bottomClique->print(Log::out());
                            Log::out() << ::std::endl;
                        }                
                        (*currentStack)[i]->link(bottomClique);
                    }
                    currentDepth = 0;
                }
            }
        }
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " after invalidateMonitors("
                       << (int) i->getId()
                       << "), depth="
                       << (int) currentDepth
                       << ::std::endl;
        }
    };
    void openMonitor(Inst *monitorEnter, Opnd *obj) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " before openMonitor(" 
                       << (int) monitorEnter->getId()
                       << ", ";
            obj->print(Log::out());
            Log::out() << "), depth="
                       << (int) currentDepth
                       << ::std::endl;
        }
        assert(currentDepth <= SYNC_STACK_TOP_DEPTH);

        SyncClique *clique = newMonitor(obj);

        monitorCliques[monitorEnter] = clique;
        (*currentStack)[currentDepth] = clique;
        currentDepth += 1;
            
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " after openMonitor(" 
                       << (int) monitorEnter->getId()
                       << ", ";
            obj->print(Log::out());
            Log::out() << "): depth="
                       << (int) currentDepth
                       << ::std::endl;
        }
    }
    void closeMonitor(Inst *monitorExit, Opnd *obj) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " before closeMonitor("
                       << (int) monitorExit->getId()
                       << ", ";
            obj->print(Log::out());
            Log::out() << "): depth="
                       << (int) currentDepth
                       << ::std::endl;
        }

        if (currentDepth > 0) {
            assert(currentStack);
            SyncClique *currentMonitor = (*currentStack)[currentDepth-1];
            currentMonitor = currentMonitor->find();
            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << " setting monitorCliques["
                           << (int) monitorExit->getId()
                           << "] = ";
                currentMonitor->print(Log::out());
                Log::out() << ::std::endl;
            }
            monitorCliques[monitorExit] = currentMonitor;

            // now, we can't check whether opnd == obj at this point, since opnd
            // may not be set yet (it may come from another block); instead, we
            // assume it is correct, but record the cliques which must be invalidated
            // or corrected (with RecCount) if it is found to be unbalanced when 
            // code is modified.

            currentDepth -= 1;
            if (currentDepth > 0) {
                assert(currentStack);
                // record cliques which depend on this opnd matching
                StlVectorSet<SyncClique *> *openSet = new (mem) StlVectorSet<SyncClique *>(mem);
                for (uint32 j=0; j < currentDepth; ++j) {
                    openSet->insert((*currentStack)[j]);
                }
                needRecCount[monitorExit] = openSet;
            }
        } else {
            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << " currentDepth is 0 at monitorExit; setting "
                           << " monitorCliques[monitorExit] = bottom" << ::std::endl;
            }
            monitorCliques[monitorExit] = bottomClique;
        }
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " after closeMonitor("
                       << (int) monitorExit->getId()
                       << ", ";
            obj->print(Log::out());
            Log::out() << "), depth="
                       << (int) currentDepth
                       << ::std::endl;
        }
    };
};

VarOpnd *
SyncOpt::getLockVar(Opnd *obj)
{
    OpndManager &opndManager = irManager.getOpndManager();
    InstFactory &instFactory = irManager.getInstFactory();
    VarOpnd *thisLockVar = lockVar[obj];
    if (!thisLockVar) {
        thisLockVar = opndManager.createVarOpnd(lockAddrType,
                                                false);

        Opnd *lockAddrTmp = opndManager.createSsaTmpOpnd(lockAddrType);
        Inst *ldLockInst = instFactory.makeLdLockAddr(lockAddrTmp, obj);

        Inst *stVarLockAddr = instFactory.makeStVar(thisLockVar, lockAddrTmp);
        
        Inst *objDef = obj->getInst();
        if (objDef->getOperation().mustEndBlock()) {
            // can't insert right after objectdef
            CFGNode *objDefNode = objDef->getNode();
            CFGEdge *outEdge = (CFGEdge*) objDefNode->getUnconditionalEdge();
            assert(outEdge);
            CFGNode *nextNode = outEdge->getTargetNode();
            Inst *nextInst = nextNode->getFirstInst()->next();
            while (nextInst->getOpcode() == Op_Phi) {
                nextInst = nextInst->next();
            }
            ldLockInst->insertBefore(nextInst);
        } else {
            ldLockInst->insertAfter(objDef);
        }
        stVarLockAddr->insertAfter(ldLockInst);

        lockVar[obj] = thisLockVar;
    }
    return thisLockVar;
}

VarOpnd *
SyncOpt::getOldValueOpnd(SyncClique *clique)
{
    VarOpnd *oldValueVar = clique->oldValueVar;
    if (!oldValueVar) {
        OpndManager &opndManager = irManager.getOpndManager();
        oldValueVar = opndManager.createVarOpnd(lockType,
                                                false);
        clique->oldValueVar = oldValueVar;
    }
    return oldValueVar;
}


// Stage 1(a): solve a DF problem to find depth of stack of open monitors at each
//    block; this gives balanced exits; returns 
//    we ignore exception edges for now
// The result is the total count of stack elements in entrySolution and exitSolution,
//    which are also initialized here

uint32 SyncOpt::findBalancedExits_Stage1(bool optimistic, bool use_IncRecCount, uint32 numNodes,
                                         SyncOptDfValue *&entrySolution, SyncOptDfValue *&exitSolution)
{
    SyncOptForwardInstance findBalancedExits(mm, optimistic, use_IncRecCount);
    
    FlowGraph &fg = irManager.getFlowGraph();
    solve<SyncOptDfValue>(&fg, findBalancedExits, true, // forwards
                          mm, 
                          entrySolution, exitSolution,
                          Log::cat_opt_sync(), true); // ignore exception edges
    uint32 numCliques = 0;
    {
        for (uint32 i = 0; i<numNodes; i++) {
            numCliques += entrySolution[i].getDepth();
            numCliques += exitSolution[i].getDepth();
        }
    }
    return numCliques;
}

void SyncOpt::linkStacks(uint32 depth1, SyncClique *stack1,
                         uint32 depth2, SyncClique *stack2,
                         SyncClique *bottomClique)
{
    // link 
    //   stack1[depth1-1],..,stack1[0],bot,..
    //   stack2[depth2-1],..,stack2[0],bot,..
    uint32 mindepth = ::std::min(depth1, depth2);
    uint32 i = 0;
    for ( ; i < mindepth; ++i) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " linking02 ";
            stack2[depth2-1-i].print(Log::out());
            Log::out() << " with ";
            stack1[depth1-1-i].print(Log::out());
            Log::out() << ::std::endl;
        }
        stack2[depth2-1-i].link(&stack1[depth1-1-i]);
    }
    for ( ; i < depth1; ++i) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " linking03 ";
            stack1[depth1-1-i].print(Log::out());
            Log::out() << " with bottom (";
            bottomClique->print(Log::out());
            Log::out() << ")" << ::std::endl;
        }
        stack1[depth1-1-i].link(bottomClique);
    }
    for ( ; i < depth2; ++i) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << " linking04 ";
            stack2[depth2-1-i].print(Log::out());
            Log::out() << " with bottom (";
            bottomClique->print(Log::out());
            Log::out() << ")" << ::std::endl;
        }
        stack2[depth2-1-i].link(bottomClique);
    }
}

// starting with cliques in inStack[0..depthIn-1], walk node with walker
// and union result with outStack[0..depthOut-1].  stackspace is available for scratch.
void SyncOpt::findBalancedExits_Stage2a(CFGNode *node,
                                        uint32 depthIn,
                                        SyncClique *inStack,
                                        uint32 depthOut,
                                        SyncClique *outStack,
                                        BuildSyncCliquesWalker &walker,
                                        StlVector<SyncClique *> &stackspace, // scratch area
                                        SyncClique *bottomClique)
{
    if (depthIn > 0) {
        assert(depthIn <= SYNC_STACK_TOP_DEPTH);
        for (uint32 i=0; i<depthIn; i++) {
            stackspace[i] = &inStack[i];
        }
    }
    assert(&walker.mem == &mm);
    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "  Before walk, depthIn=" << (int) depthIn 
                   << ", stack = ";
        for (uint32 i=0; i<depthIn; i++) {
            stackspace[i]->print(Log::out());
            Log::out() << " ";
        }
        Log::out() << ::std::endl;
    }
    assert(&walker.mem == &mm);
    walker.startNode(&stackspace, depthIn);
    assert(&walker.mem == &mm);
    WalkInstsInBlock<true, BuildSyncCliquesWalker>(node, walker); // in order
    
    assert(&walker.mem == &mm);
    uint32 currentDepth = walker.getDepth();
    StlVector<SyncClique *> *currentStack = walker.getStack();
    assert(currentStack);
    
    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "  After walk of node #"
                   << (int) node->getId()
                   << ", currentDepth=" 
                   << (int) currentDepth
                   << ", stack = ";
        for (uint32 i=0; i<currentDepth; i++) {
            (*currentStack)[i]->print(Log::out());
            Log::out() << " ";
        }
        Log::out() << ::std::endl;
    }
    
    uint32 mindepth = ::std::min(currentDepth, depthOut);
    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "  Linking05 with outstack of depth " 
                   << (int) depthOut
                   << ", outstack = ";
        for (uint32 i=0; i<currentDepth; i++) {
            outStack[i].print(Log::out());
            Log::out() << " ";
        }
        Log::out() << ::std::endl;
    }
    {
        uint32 i = 0;
        for ( ; i < mindepth; ++i) {
            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << " linking06 ";
                (*currentStack)[currentDepth-1-i]->print(Log::out());
                Log::out() << " with ";
                outStack[depthOut-1-i].print(Log::out());
                Log::out() << ")" << ::std::endl;
            }
            (*currentStack)[currentDepth-1-i]->link(&outStack[depthOut-1-i]);
        }
        for ( ; i < currentDepth; ++i) {
            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << " linking07 ";
                (*currentStack)[currentDepth-1-i]->print(Log::out());
                Log::out() << " with bottom (";
                bottomClique->print(Log::out());
                Log::out() << ")" << ::std::endl;
            }
            (*currentStack)[currentDepth-1-i]->link(bottomClique);
        }
        for ( ; i < depthOut; ++i) {
            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << " linking08 ";
                outStack[depthOut-1-i].print(Log::out());
                Log::out() << " with bottom (";
                bottomClique->print(Log::out());
                Log::out() << ")" << ::std::endl;
            }
            outStack[depthOut-1-i].link(bottomClique);
        }
    }
}

// Stage 2: associate a SyncClique with every open monitor, apply nodes to unify
//    cliques with appropriate successors, and associate a clique with each 
//    monitor operation
//    now we ignore exception edges out of monitorexit nodes unless optimistic
void SyncOpt::findBalancedExits_Stage2(bool optimistic, bool use_IncRecCount,
                                       SyncOptDfValue *entrySolution,
                                       SyncOptDfValue *exitSolution,
                                       SyncClique *bottomClique,
                                       SyncClique **entryCliques, SyncClique **exitCliques,
                                       StlMap<Inst *, StlVectorSet<SyncClique *> *> &needRecCount,
                                       StlMap<Inst *, SyncClique *> &monitorCliques)
{
    BuildSyncCliquesWalker walker(mm, bottomClique, monitorCliques,
                                  optimistic, use_IncRecCount,
                                  needRecCount);
    StlVector<SyncClique *>stackspace(mm, SYNC_STACK_TOP_DEPTH+1, 0);
        
    FlowGraph &fg = irManager.getFlowGraph();
    const CFGNodeDeque &nodes =  fg.getNodes();
    assert(&walker.mem == &mm);
    CFGNodeDeque::const_iterator 
        iter = nodes.begin(),
        end = nodes.end();
    for (; iter != end; ++iter) {
        CFGNode *node = *iter;
        uint32 nodeId = node->getId();
        uint32 depthIn = entrySolution[nodeId].getDepth();
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "Considering node " << (int) nodeId 
                       << " with depthIn=" << (int) depthIn
                       << ::std::endl;
        }
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            if (!entrySolution[nodeId].isTop()) {
                Log::out() << "Node solution is not Top" << ::std::endl;
            } else {
                Log::out() << "Node solution is Top" << ::std::endl;
            }
        }
        // propagate node solution through node to exits
        
        uint32 depthOut = exitSolution[nodeId].getDepth();
        assert(&walker.mem == &mm);
        findBalancedExits_Stage2a(node, 
                                  depthIn, entryCliques[nodeId],
                                  depthOut, exitCliques[nodeId],
                                  walker, stackspace, bottomClique);
        
        // link with every successor except for exception edges from MonitorExit
        const CFGEdgeDeque &outEdges = node->getOutEdges();
        CFGEdgeDeque::const_iterator
            eiter = outEdges.begin(),
            eend = outEdges.end();
        for ( ; eiter != eend; ++eiter) {
            CFGEdge *e = *eiter;
            CFGNode *target = e->getTargetNode();
            
            // skip any exception edge from a MonitorExit unless optimistic
            if (!optimistic && target->isDispatchNode()) {
                Inst *lastInst = node->getLastInst();
                if ((lastInst->getOpcode() == Op_TauMonitorExit) ||
                    (lastInst->getOpcode() == Op_OptimisticBalancedMonitorExit)) {
                    
                    if (Log::cat_opt_sync()->isDebugEnabled()) {
                        Log::out() << "Skipping monitor exception edge from node #" 
                                   << (int) nodeId
                                   << " to dispatch node #"
                                   << (int) target->getId()
                                   << ::std::endl;
                    }            
                    
                    // skip this edge
                    continue;
                }
            }
            
            // link with successor inputs
            uint32 targetId = target->getId();
            SyncClique *targetStack = entryCliques[targetId];
            uint32 targetDepth = entrySolution[targetId].getDepth();

            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << "Linking09 output of node #" 
                           << (int) nodeId
                           << " with input of node #"
                           << (int) targetId
                           << ::std::endl;
            }            
            linkStacks(depthOut, exitCliques[nodeId],
                       targetDepth, targetStack,
                       bottomClique);
        }
    }
    // link any open monitor at the exit node with bottom
    {
        CFGNode *exitNode = fg.getExit();
        uint32 exitId = exitNode->getId();
        uint32 depthIn = entrySolution[exitId].getDepth();
        SyncClique *exitStack = entryCliques[exitId];
        for (uint32 i=0; i< depthIn; ++i) {
            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << " for exit node: i="
                           << (int) i;
                Log::out() << " linking10 ";
                exitStack[i].print(Log::out());
                Log::out() << " to bottom (";
                bottomClique->print(Log::out());
                Log::out() << ")" << ::std::endl;
            }                
            exitStack[i].link(bottomClique);
        }
    }        
}

bool SyncOpt::monitorExitIsBad(Inst *monExit, SyncClique *clique, 
                               SyncClique *cliqueRoot, SyncClique *bottomCliqueRoot) {
    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "Considering monitorExit instruction: ";
        monExit->print(Log::out());
        Log::out() << " with ";
        clique->print(Log::out());
        Log::out() << ", with root = ";
        cliqueRoot->print(Log::out());
        Log::out() << ::std::endl;
    }
    
    if (cliqueRoot == bottomCliqueRoot) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "  clique == bottom" << ::std::endl;
        }
        return true;
    } else if (!cliqueRoot->opnd) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "  cliqueRoot->opnd = NULL" << ::std::endl;
        }
        return true;
    } else if (cliqueRoot->opnd != monExit->getSrc(0)) {
        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "  clique->opnd != monitorExit->opnd:" << ::std::endl;
            Log::out() << "    Clique=";
            if (cliqueRoot->opnd)
                cliqueRoot->opnd->print(Log::out());
            else
                Log::out() << "NULL";
            Log::out() << "; obj = ";
            monExit->getSrc(0)->print(Log::out());
            Log::out() << ::std::endl;
        }
        return true;
    }
    return false;
}

// stage 3a:
//   iteratively find bad monitor Insts
//   on each iteration,
//      find new bad monitor exit Insts (those whose clique =~ bottom, which haven't been processed)
//      union their cliques with bottom
//      also union their dependent monitors with bottom
//      add the effect of the exception edge from the bad monitor exit (may union more with bottom)
//   repeat until it is stable
void SyncOpt::findBalancedExits_Stage3(bool optimistic, bool use_IncRecCount,
                                       SyncOptDfValue *entrySolution,
                                       SyncOptDfValue *exitSolution,
                                       SyncClique *bottomClique,
                                       SyncClique **entryCliques, SyncClique **exitCliques,
                                       StlMap<Inst *, StlVectorSet<SyncClique *> *> &needRecCount,
                                       StlMap<Inst *, SyncClique *> &monitorCliques)
{
    // first scan all monitor instructions and make sure their Opnd matches
    // match their clique's Opnd; if not, link to bottom.
    {
        StlMap<Inst *, SyncClique *>::iterator
            miter = monitorCliques.begin(),
            mend = monitorCliques.end();
        for ( ; miter != mend; ++miter) {
            Inst *inst = (*miter).first;
            SyncClique *clique = (*miter).second;
            
            Opnd *obj = inst->getSrc(0);
            clique = clique->find();
            if (clique->opnd) {
                if (clique->opnd != obj) {
                    clique->link(bottomClique);
                }
            } else {
                clique->opnd = obj;
            }
        }            
        bottomClique = bottomClique->find();
    }
    // now any monitor inst in a clique with another monitor inst with 
    // un-matching opnd has been linked to bottom.

    StlVectorSet<SyncClique *> badMonitors(mm);
    
    StlVectorSet<SyncClique *> dependentOnBadMonitors(mm);
    StlVectorSet<Inst *> badMonitorExits(mm);
    StlVector<Inst *> newBadMonitorExits(mm);
    SyncClique *bottomCliqueRoot = bottomClique->find();
    
    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << " beginning Stage3" << ::std::endl;
    }

    bool needToCheckAgain = true;
    while (needToCheckAgain) {
        needToCheckAgain = false;

        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "  beginning Stage3 iteration" << ::std::endl;
        }

        StlMap<Inst *, SyncClique *>::iterator
            miter = monitorCliques.begin(),
            mend = monitorCliques.end();
        for ( ; miter != mend; ++miter) {
            Inst *inst = (*miter).first;
            if ((inst->getOpcode() != Op_TauMonitorExit) &&
                (inst->getOpcode() != Op_OptimisticBalancedMonitorExit))
                continue;
            
            SyncClique *clique = (*miter).second;
            SyncClique *cliqueRoot = clique->find();

            if (badMonitorExits.has(inst)) {
                continue;
            }
            bool badclique = false;
            
            badclique = monitorExitIsBad(inst, clique, cliqueRoot, bottomCliqueRoot);
            
            //   at this point, opnd should be set for each clique root; check whether
            //   monitorExit opnd matches the clique opnd, if not, add it to newBadMonitorExits.
            if (badclique) {
                newBadMonitorExits.push_back(inst);
                    
                //  process any exception edge to merge monitors.  If anything changed, we 
                //  will have to re-process everything.
                
                StlMap<Inst *, StlVectorSet<SyncClique *> *>::iterator
                    foundDeps = needRecCount.find(inst),
                    endDeps = needRecCount.end();
                if (foundDeps != endDeps) {
                    StlVectorSet<SyncClique *> *deps = (*foundDeps).second;
                    dependentOnBadMonitors.insert(deps->begin(), deps->end());
                }
            }
        }

        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "  done Stage3a" << ::std::endl;
        }

        // union all new badMonitorExits with bottom
        StlVectorSet<Inst *>::iterator
            monExitIter = newBadMonitorExits.begin(),
            monExitEnd = newBadMonitorExits.end();
        for ( ; monExitIter != monExitEnd; ++monExitIter) {
            Inst *monExit = *monExitIter;
            
            SyncClique *clique = monitorCliques[monExit];
            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << "  linking11 badMonitor ";
                clique->print(Log::out());
                Log::out() << " with bottom (";
                bottomClique->print(Log::out());
                Log::out() << ")" << ::std::endl;
            }                
            bottomCliqueRoot->link(clique);
        }
        bottomCliqueRoot = bottomCliqueRoot->find();

        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "  done Stage3b" << ::std::endl;
        }

        // merge any new dependent monitors from dependentOnBadMonitors
        bool foundDependentMonitor = false;
        StlVectorSet<SyncClique *>::iterator
            depMonIter = dependentOnBadMonitors.begin(),
            depMonEnd = dependentOnBadMonitors.end();
        for ( ; depMonIter != depMonEnd; ++depMonIter) {
            SyncClique *depMonitor = *depMonIter;
            SyncClique *depMonRoot = depMonitor->find();
            if (depMonRoot != bottomCliqueRoot) {
                foundDependentMonitor = true;                // we need to re-iterate

                if (Log::cat_opt_sync()->isDebugEnabled()) {
                    Log::out() << "  linking12 depMonRoot ";
                    depMonRoot->print(Log::out());
                    Log::out() << " with bottom (";
                    bottomCliqueRoot->print(Log::out());
                    Log::out() << ")" << ::std::endl;
                }                

                bottomCliqueRoot->link(depMonRoot);
                bottomCliqueRoot = bottomCliqueRoot->find(); // may have changed
            }
        }

        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "  done Stage3c" << ::std::endl;
        }

        // add effect of the exception edge from each newly bad exit,
        // unless optimistic (in which case we included it previously)
        bool differs = foundDependentMonitor;
        if (!optimistic) {
            StlVectorSet<Inst *>::iterator 
                newBadExitIter = newBadMonitorExits.begin(),
                newBadExitEnd = newBadMonitorExits.end();
            for ( ; newBadExitIter != newBadExitEnd; ++newBadExitIter) {
                Inst *newBadExit = *newBadExitIter;
                CFGNode *node = newBadExit->getNode();
                uint32 nodeId = node->getId();
                uint32 nodeDepth = exitSolution[nodeId].getDepth();
                
                CFGEdge *e = (CFGEdge *)node->getExceptionEdge();
                CFGNode *target = e->getTargetNode();
                uint32 targetId = target->getId();
                uint32 targetDepth = entrySolution[targetId].getDepth();
                
                SyncClique *nodeStack = exitCliques[nodeId];
                SyncClique *targetStack = entryCliques[targetId];
                if (!differs) {
                    if (targetDepth != nodeDepth) {
                        differs = true;
                    } else {
                        for (uint32 i = 0; i < targetDepth; ++i) {
                            if (nodeStack[i].find() != targetStack[i].find()) {
                                differs = true;
                                break;
                            }
                        }
                    }
                }
                if (Log::cat_opt_sync()->isDebugEnabled()) {
                    Log::out() << "Linking13 exception edge from bad monexit at node #" 
                               << (int) nodeId
                               << " to dispatch node #"
                               << (int) targetId
                               << ::std::endl;
                }            
                linkStacks(nodeDepth, nodeStack,
                           targetDepth, targetStack,
                           bottomCliqueRoot);
            }            
        }

        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "  done Stage3d" << ::std::endl;
        }

        badMonitorExits.insert(newBadMonitorExits.begin(), newBadMonitorExits.end());
        newBadMonitorExits.clear();
        
        needToCheckAgain = differs;

        if (Log::cat_opt_sync()->isDebugEnabled()) {
            Log::out() << "  done Stage3 iteration" << ::std::endl;
        }
    }
}


void SyncOpt::findBalancedExits(bool optimistic, bool use_IncRecCount)
{
    OpndManager &opndManager = irManager.getOpndManager();
    InstFactory &instFactory = irManager.getInstFactory();

    FlowGraph &fg = irManager.getFlowGraph();
    uint32 numNodes = fg.getMaxNodeId();
    SyncOptDfValue *entrySolution, *exitSolution;
    uint32 numCliques = findBalancedExits_Stage1(optimistic, use_IncRecCount, numNodes,
                                                 entrySolution, exitSolution);

    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "numNodes is " << (int) numNodes << ::std::endl;
        Log::out() << "numCliques is " << (int) numCliques << ::std::endl;
    }

    SyncClique *cliquesHeap = new (mm) SyncClique[numCliques];
    SyncClique **entryCliques = new (mm) SyncClique *[numNodes];
    SyncClique **exitCliques = new (mm) SyncClique *[numNodes];
    uint32 cliqueHeapIndex = 0;
    {
        for (uint32 i = 0; i<numNodes; i++) {
            uint32 lin = entrySolution[i].getDepth();
            uint32 lout = exitSolution[i].getDepth();

            entryCliques[i] = &cliquesHeap[cliqueHeapIndex];
            cliqueHeapIndex += lin;
            exitCliques[i] = &cliquesHeap[cliqueHeapIndex];
            cliqueHeapIndex += lout;

            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << "  node " << (int) i << " has " << (int) lin << " cliques in: [ ";
                for (uint32 j = 0; j<lin; ++j) { 
                    entryCliques[i][j].print(Log::out());
                    Log::out() << " ";
                }
                Log::out() << "]" << ::std::endl;
                
                Log::out() << "  node " << (int) i << " has " << (int) lout << " cliques out: [ ";
                for (uint32 jout = 0; jout<lout; ++jout) { 
                    exitCliques[i][jout].print(Log::out());
                    Log::out() << " ";
                }
                Log::out() << "]" << ::std::endl;
            }
        }
    }
    assert(cliqueHeapIndex <= numCliques);

    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "finished dataflow solution" << ::std::endl;
    }

    SyncClique *bottomClique = new (mm) SyncClique();
    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "Bottom clique is ";
        bottomClique->print(Log::out());
        Log::out() << ::std::endl;
    }
    
    // process each node and edge (except exception edges) to union monitor cliques
    StlMap<Inst *, StlVectorSet<SyncClique *> *> needRecCount(mm);
    StlMap<Inst *, SyncClique *> monitorCliques(mm);
    findBalancedExits_Stage2(optimistic, use_IncRecCount,
                             entrySolution, exitSolution,
                             bottomClique, entryCliques, exitCliques,
                             needRecCount, monitorCliques);

    // now, each monitorEnter/Exit has an associated clique,
    // and we have applied UnionFind::link to them appropriately (ignoring monexit exceptions)
    // if usereccount, then needRecCount[invalidateInst]=cliques to be incremented
    //    before the invalidating instruction
    // also, needRecCount[monitorExit]=cliques to be invalidated if monitorExit opnd
    //    doesn't match, or if the monitorExit clique is/was invalidated
    SyncClique *bottomCliqueRoot = bottomClique->find();
    if (Log::cat_opt_sync()->isDebugEnabled()) {
        Log::out() << "Bottom clique root is ";
        bottomCliqueRoot->print(Log::out());
        Log::out() << ::std::endl;
    }

    // stage 3a

    // stage 3b:
    //   at this point, opnd should be set for each clique root; check whether
    //   monitorExit opnd matches the clique opnd, if not, add it to newBadMonitorExits.

    findBalancedExits_Stage3(optimistic, use_IncRecCount,
                             entrySolution, exitSolution,
                             bottomClique, entryCliques, exitCliques,
                             needRecCount, monitorCliques);
                             
    // at this point, a monitor inst monInst is unbalanced if monitorCliques[monInst] =~ bottom

    // stage 3: Now that we know which cliques are balanceable, modify code.
    //   Other cliques we don't bother with.
    {
        StlMap<Inst *, SyncClique *>::iterator
            miter = monitorCliques.begin(),
            mend = monitorCliques.end();
        for ( ; miter != mend; ++miter) {
            Inst *inst = (*miter).first;
            SyncClique *clique = (*miter).second;

            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << "Considering balancing instruction: ";
                inst->print(Log::out());
                Log::out() << " with ";
                clique->print(Log::out());
                Log::out() << ::std::endl;
            }

            clique = clique->find();
            if ((clique == bottomCliqueRoot)) {
                if (Log::cat_opt_sync()->isDebugEnabled()) {
                    Log::out() << "  Clique == bottom" << ::std::endl;
                }
                continue;
            }
            {
                // we can balance it
                Opnd *obj = inst->getSrc(0);
                assert(obj == clique->opnd);

                VarOpnd *lockVar = getLockVar(obj);
                VarOpnd *oldValueVar = getOldValueOpnd(clique);

                Opnd *lockAddrTmp = opndManager.createSsaTmpOpnd(lockAddrType);
                Opnd *oldValueTmp = opndManager.createSsaTmpOpnd(lockType);

                Opnd *tau = 0;
                switch (inst->getOpcode()) {
                case Op_TauOptimisticBalancedMonitorEnter:
                    if (optimistic) break; // we can ignore this if optimistic, otherwise try to convert it to unoptimistic balmonenter
                    tau = inst->getSrc(2);
                case Op_TauMonitorEnter:
                    {
                        if (!tau)
                            tau = inst->getSrc(1);
                        assert(tau->getType()->tag == Type::Tau);

                        Inst *ldVarLockAddr = instFactory.makeLdVar(lockAddrTmp, lockVar);
                        Inst *balmonenter = (optimistic
                                             ? instFactory.makeTauOptimisticBalancedMonitorEnter(oldValueTmp,
                                                                                                 obj,
                                                                                                 lockAddrTmp,
                                                                                                 tau)
                                             : instFactory.makeTauBalancedMonitorEnter(oldValueTmp,
                                                                                       obj,
                                                                                       lockAddrTmp,
                                                                                       tau));
                        Inst *stVarOldValue = instFactory.makeStVar(oldValueVar, oldValueTmp);
                        ldVarLockAddr->insertAfter(inst);
                        balmonenter->insertAfter(ldVarLockAddr);
                        stVarOldValue->insertAfter(balmonenter);
                        inst->unlink();
                    }
                    break;
                case Op_OptimisticBalancedMonitorExit:
                    if (optimistic) break; // we can ignore this if optimistic, otherwise convert it to un-optimistic balmonexit
                case Op_TauMonitorExit:
                    {
                        Inst *ldVarLockAddr = instFactory.makeLdVar(lockAddrTmp, lockVar);
                        Inst *ldVarOldValue = instFactory.makeLdVar(oldValueTmp, 
                                                                    oldValueVar);
                        Inst *balmonexit = (optimistic
                                            ? instFactory.makeOptimisticBalancedMonitorExit(obj,
                                                                                            lockAddrTmp,
                                                                                            oldValueTmp)
                                            : instFactory.makeBalancedMonitorExit(obj,
                                                                                  lockAddrTmp,
                                                                                  oldValueTmp));
                        ldVarLockAddr->insertAfter(inst);
                        ldVarOldValue->insertAfter(ldVarLockAddr);
                        balmonexit->insertAfter(ldVarOldValue);

                        if (!optimistic) {
                            // remove exception edge
                            CFGNode *node = inst->getNode();
                            CFGEdge* edge = (CFGEdge *)node->getExceptionEdge();
                            assert(edge != NULL);
                            fg.removeEdge(edge);
                        }
                        
                        inst->unlink();
                    }
                    break;
		default:
		    break;
                }
            }
        }
    }        
    if (use_IncRecCount) {
        StlVectorSet<SyncClique *> doneCliques(mm);
        StlMap<Inst *, StlVectorSet<SyncClique *> *>::iterator
            miter = needRecCount.begin(),
            mend = needRecCount.end();
        for ( ; miter != mend; ++miter) {
            Inst *invalidatingInst = (*miter).first;
            StlVectorSet<SyncClique *> *cliques = (*miter).second;
            assert(cliques);
            
            if (Log::cat_opt_sync()->isDebugEnabled()) {
                Log::out() << "Considering increccount for instruction: ";
                invalidatingInst->print(Log::out());
                Log::out() << ::std::endl;
            }

            doneCliques.clear();

            StlVectorSet<SyncClique *>::iterator
                cliter = cliques->begin(),
                clend = cliques->end();
            for ( ; cliter != clend; ++cliter) {
                SyncClique *thisClique = *cliter;
                thisClique = thisClique->find();
                
                if ((thisClique != bottomCliqueRoot) &&
                    !doneCliques.has(thisClique)) {
                    doneCliques.insert(thisClique);

                    Opnd *obj = thisClique->opnd;
                    assert(obj);

                    // insert incrreccount
                    VarOpnd *oldValueVar = getOldValueOpnd(thisClique);
                    Opnd *oldValueTmp = opndManager.createSsaTmpOpnd(lockType);
                    Inst *ldVarOldValue = instFactory.makeLdVar(oldValueTmp, 
                                                                oldValueVar);
                    Inst *increcInst = instFactory.makeIncRecCount(obj, oldValueTmp);
                    
                    ldVarOldValue->insertBefore(invalidatingInst);
                    increcInst->insertAfter(ldVarOldValue);
                }
            }
        }
    }
}

void SyncOpt::insertUnwindMonitorExit(Opnd *syncMethodOpnd,
                                      CFGNode *&tmpDispatchNode, CFGNode *&tmpCatchNode,
                                      CFGNode *&tmpRethrowNode)
{
    assert(flags.optimistic);
    assert(syncMethodOpnd);
    InstFactory &instFactory = irManager.getInstFactory();
    OpndManager &opndManager = irManager.getOpndManager();
    TypeManager &typeManager = irManager.getTypeManager();
    FlowGraph &fg = irManager.getFlowGraph();
    CFGNode *unwind = fg.getUnwind();
    assert(unwind); // use it if we have an unwind node only

    CFGNode *newDispatchNode = fg.createDispatchNode();

    Opnd* ex = opndManager.createSsaTmpOpnd(typeManager.getSystemObjectType());
    CFGNode *newCatchNode = fg.createCatchNode(0, ex->getType());
    newCatchNode->append(instFactory.makeCatch(ex));
    bool insertedMethodExit = false;

    CFGNode *rethrowNode = fg.createBlockNode();
    rethrowNode->append(instFactory.makeThrow(Throw_NoModifier, ex));
    
    // Redirect exception exits that aren't from a node whose successor's successor
    // is return, to newDispatchNode
    const CFGEdgeDeque& unwindEdges = unwind->getInEdges();
    CFGEdgeDeque::const_iterator eiter;
    for(eiter = unwindEdges.begin(); eiter != unwindEdges.end(); ) {
        bool redirectEdge = true;
        CFGEdge* edge = *eiter;
        ++eiter;
        CFGNode *source = edge->getSourceNode();
        
        // check whether source is a return-path monitorExit(this)
        Inst *sourceLastInst = source->getLastInst();
        if (((sourceLastInst->getOpcode() == Op_TauMonitorExit) ||
             (sourceLastInst->getOpcode() == Op_OptimisticBalancedMonitorExit)) &&
            (sourceLastInst->getSrc(0) == syncMethodOpnd)) {
            // is a monitorExit(this)
            
            if (!insertedMethodExit) {
                insertedMethodExit = true;
                if (sourceLastInst->getOpcode() == Op_TauMonitorExit) {
                    Opnd *tauSrcNonNull = sourceLastInst->getSrc(1);
                    assert(tauSrcNonNull->getType()->tag == Type::Tau);
                    newCatchNode->append(instFactory.makeTauMonitorExit(syncMethodOpnd, tauSrcNonNull));
                } else {
                    Opnd *lockAddrOpnd = sourceLastInst->getSrc(1);
                    Opnd *oldValueOpnd = sourceLastInst->getSrc(1);
                    newCatchNode->append(instFactory.makeOptimisticBalancedMonitorExit(syncMethodOpnd,
                                                                                       lockAddrOpnd,
                                                                                       oldValueOpnd));
                }
            }
            
            // now check whether other successor is a return node
            const CFGEdgeDeque& sourceEdges = source->getOutEdges();
            CFGEdgeDeque::const_iterator 
                sourceEdgesIter = sourceEdges.begin(),
                sourceEdgesEnd = sourceEdges.end();
            for(; sourceEdgesIter != sourceEdgesEnd; ++sourceEdgesIter) {
                CFGEdge *sourceEdge = *sourceEdgesIter;
                CFGNode *target = sourceEdge->getTargetNode();
                if (target == unwind) continue;
                
                // check if it ends in return
                Inst *targetLastInst = target->getLastInst();
                if (targetLastInst->getOpcode() == Op_Return) {
                    // do not redirect
                    redirectEdge = false;
                    break;
                }
            }
            
        }            
        // otherwise, redirect the edge.
        if (redirectEdge)
            fg.replaceEdgeTarget(edge, newDispatchNode);
    }
    if (!insertedMethodExit) {

    }

    fg.addEdge(newDispatchNode, newCatchNode);
    fg.addEdge(newCatchNode, rethrowNode);
    fg.addEdge(newCatchNode, unwind);
    fg.addEdge(rethrowNode, unwind);

    tmpDispatchNode = newDispatchNode;
    tmpCatchNode = newCatchNode;
    tmpRethrowNode = rethrowNode;
}

void SyncOpt::removeUnwindMonitorExit(Opnd *syncMethodOpnd, 
                                      CFGNode *tmpDispatchNode, CFGNode *tmpCatchNode,
                                      CFGNode *tmpRethrowNode)
{
    assert(flags.optimistic);
    assert(syncMethodOpnd);
    FlowGraph &fg = irManager.getFlowGraph();
    CFGNode *unwind = fg.getUnwind();

    const CFGEdgeDeque& tmpDispatchInEdges = tmpDispatchNode->getInEdges();
    CFGEdgeDeque::const_iterator eiter;
    for(eiter = tmpDispatchInEdges.begin(); eiter != tmpDispatchInEdges.end(); ) {
        CFGEdge* edge = *eiter;
        ++eiter;
        fg.replaceEdgeTarget(edge, unwind);
    }

    fg.removeEdge(tmpDispatchNode, tmpCatchNode);
    fg.removeEdge(tmpCatchNode, tmpRethrowNode);
    fg.removeEdge(tmpCatchNode, unwind);
    fg.removeEdge(tmpRethrowNode, unwind);
    fg.removeNode(tmpDispatchNode);
    fg.removeNode(tmpCatchNode);
    fg.removeNode(tmpRethrowNode);
}



} //namespace Jitrino 
