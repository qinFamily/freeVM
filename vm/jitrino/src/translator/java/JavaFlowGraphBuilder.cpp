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
 * @author Intel, George A. Timoshenko
 * @version $Revision: 1.17.8.2.4.4 $
 *
 */

#include "Log.h"
#include "Opcode.h"
#include "Type.h"
#include "Stack.h"
#include "IRBuilder.h"
#include "ExceptionInfo.h"
#include "JavaFlowGraphBuilder.h"
#include "TranslatorIntfc.h"
#include "irmanager.h"
#include "FlowGraph.h"

#include <stdlib.h>
#include <string.h>

namespace Jitrino {


Node* JavaFlowGraphBuilder::genBlock(LabelInst* label) {
    if (currentBlock == NULL) {
        currentBlock = createBlockNodeOrdered(label);
        assert(fg->getEntryNode()==NULL);
        fg->setEntryNode(currentBlock);
    } else {
        currentBlock = createBlockNodeOrdered(label);
    }
    return currentBlock;
}

Node* JavaFlowGraphBuilder::genBlockAfterCurrent(LabelInst *label) {
    // hidden exception info
    void *exceptionInfo = ((LabelInst*)currentBlock->getFirstInst())->getState();
    currentBlock = createBlockNodeAfter(currentBlock, label);
    label->setState(exceptionInfo);
    return currentBlock;
}



void JavaFlowGraphBuilder::edgeForFallthrough(Node* block) {
    //
    // find fall through basic block; skip handler nodes
    //
    NodeList::const_iterator niter = std::find(fallThruNodes.begin(), fallThruNodes.end(), block);
    assert(niter != fallThruNodes.end());
    TranslatorFlags* translatorFlags = irBuilder.getTranslatorFlags();
    for(++niter; niter != fallThruNodes.end(); ++niter) {
        Node* node = *niter;
        if (translatorFlags->newCatchHandling) {
            if (node->isBlockNode() && !((LabelInst*)node->getFirstInst())->isCatchLabel()) {
                break;
            }
        } else if (node->isBlockNode()) {
                break;
        }
    }
    assert(niter != fallThruNodes.end());
    Node* fallthrough = *niter;
    addEdge(block,fallthrough);
}

Node * JavaFlowGraphBuilder::edgesForBlock(Node* block) {
    //
    // find if this block has any region that could catch the exception
    //
    Node *dispatch = NULL;
    ExceptionInfo *exceptionInfo = (CatchBlock*)((LabelInst*)block->getFirstInst())->getState();
    if (exceptionInfo != NULL) {
        dispatch = exceptionInfo->getLabelInst()->getNode();
    }else{
        dispatch = fg->getUnwindNode();
    }
    assert(dispatch->isDispatchNode());

    //
    // split the block so that 
    //      each potentially-exceptional instruction ends a block
    //
    Inst* first = (Inst*)block->getFirstInst();
    Inst* last = (Inst*)block->getLastInst();
    Inst* lastExceptionalInstSeen = NULL;
    for (Inst* inst = first->getNextInst(); inst != NULL; inst = inst->getNextInst()) {
        if (lastExceptionalInstSeen != NULL) {
            // start a new basic block
            Node *newblock = createBlockNodeAfter(block); 
            for (Inst *ins = lastExceptionalInstSeen->getNextInst(), *nextIns = NULL; ins!=NULL; ins = nextIns) {
                nextIns = ins->getNextInst();
                ins->unlink();
                newblock->appendInst(ins);
            }

            // now fix up the CFG, duplicating edges
            if (!lastExceptionalInstSeen->isThrow())
                fg->addEdge(block,newblock);
            //
            // add an edge to handler entry node
            //
            assert(!block->findTargetEdge(dispatch));
            fg->addEdge(block,dispatch);
            block = newblock;
            lastExceptionalInstSeen = NULL;
        } 
        if (inst->getOperation().canThrow()) {
            lastExceptionalInstSeen = inst;
        }
    }

    // 
    // examine the last instruction and create appropriate CFG edges
    //
    switch(last->getOpcode()) {
    case Op_Jump:
        {
        fg->addEdge(block,((BranchInst*)last)->getTargetLabel()->getNode());
        last->unlink();
        }
        break;
    case Op_Branch:
    case Op_JSR:
        addEdge(block, ((BranchInst*)last)->getTargetLabel()->getNode());
        edgeForFallthrough(block);
        break;
    case Op_Throw:
    case Op_ThrowSystemException:
    case Op_ThrowLinkingException:
        // throw/rethrow creates an edge to a handler that catches the exception
        assert(dispatch != NULL);
        assert(lastExceptionalInstSeen == last);
        break;
    case Op_Return:
        addEdge(block, fg->getReturnNode());
        break;
    case Op_Ret:
        break; // do  not do anything
    case Op_Switch:
        {
            SwitchInst *sw = (SwitchInst*)last;
            uint32 num = sw->getNumTargets();
            for (uint32 i = 0; i < num; i++) {
                Node* target = sw->getTarget(i)->getNode();
                // two switch values may go to the same block
                if (!block->findTargetEdge(target)) {
                    fg->addEdge(block,target);
                }
            }
            Node* target = sw->getDefaultTarget()->getNode();
            if (!block->findTargetEdge(target)) {
                fg->addEdge(block,target);
            }
        }
        break;
    default:;
        if (block != fg->getReturnNode()) { // a fallthrough edge is needed
           // if the basic block does not have any outgoing edge, add one fall through edge
           if (block->getOutEdges().empty())
               edgeForFallthrough(block);
        }
    }
    //
    // add an edge to handler entry node
    //
    if (lastExceptionalInstSeen != NULL &&  !block->findTargetEdge(dispatch))
        fg->addEdge(block,dispatch);
    return block;
}

void JavaFlowGraphBuilder::edgesForHandler(Node* entry) {
    CatchBlock *catchBlock = (CatchBlock*)((LabelInst*)entry->getFirstInst())->getState();
    if(entry == fg->getUnwindNode())
        // No local handlers for unwind.
        return;
    //
    // create edges between handler entry and each catch
    //
    CatchHandler * handlers = catchBlock->getHandlers();
    for (;handlers != NULL; handlers = handlers->getNextHandler())  {
        fg->addEdge(entry,handlers->getLabelInst()->getNode());
    }
    // edges for uncaught exception
    ExceptionInfo *nextBlock = NULL;
    for (nextBlock = catchBlock->getNextExceptionInfoAtOffset();
        nextBlock != NULL; nextBlock = nextBlock->getNextExceptionInfoAtOffset()) 
    {
        if (nextBlock->isCatchBlock()) {
            Node *next = nextBlock->getLabelInst()->getNode();
            fg->addEdge(entry,next);
            break;
        }
    }
    if(nextBlock == NULL) {
        fg->addEdge(entry,fg->getUnwindNode());
    }
}

void JavaFlowGraphBuilder::createCFGEdges() {
    for (NodeList::iterator it = fallThruNodes.begin(); it!=fallThruNodes.end(); ++it) {
        Node* node = *it;
        if (node->isBlockNode())
            node = edgesForBlock(node);
        else if (node->isDispatchNode())
            edgesForHandler(node);
        else
            assert(node == fg->getExitNode());
    }
}

#define WHITE 0  // node has not been touched yet
#define BLACK 1  // node can reach exit node

//
// Traverse all the nodes in a reverse DFS manner, and mark all the ndoes
// that are reachable from exit node with BLACK.
//
void JavaFlowGraphBuilder::reverseDFS( StlVector<int8>& state,
                                       Node* targetNode,
                                       uint32* nodesCounter )
{
    const Edges& in_edges = targetNode->getInEdges();

    assert( state[targetNode->getId()] == WHITE );
    state[targetNode->getId()] = BLACK;

    *nodesCounter += 1;

    for( Edges::const_iterator in_iter = in_edges.begin(); in_iter != in_edges.end(); in_iter++ ){
        Node* srcNode = (*in_iter)->getSourceNode();

        if( state[srcNode->getId()] != BLACK ){
            reverseDFS( state, srcNode, nodesCounter );
        }
    }

    return;
}

//
// Traverse all the nodes in a forward DFS manner, and add dispatches
// for while(true){;} loops, if they exist.
//
void JavaFlowGraphBuilder::forwardDFS( StlVector<int8>& state,
                                       Node* srcNode,
                                       Edges& edgesForSplicing)
{
    // Flip the state, so the same node will not be visited more than twice.
    state[srcNode->getId()] = ~state[srcNode->getId()];

    const Edges& out_edges = srcNode->getOutEdges();

    for( Edges::const_iterator out_iter = out_edges.begin(); out_iter != out_edges.end(); out_iter++ ){

        Node* targetNode = (*out_iter)->getTargetNode();
        const int32 targetState = state[targetNode->getId()];

        if( targetState == BLACK ||
            targetState == WHITE ){
            // Keep searching ...
            forwardDFS( state, targetNode, edgesForSplicing );
        } else if( targetState == ~WHITE){
            state[targetNode->getId()] = ~BLACK;  // Now it can reach exit node.
            edgesForSplicing.push_back((*out_iter));
        }
    }

    // Don't visit ~WHITE node again.
    state[srcNode->getId()] = ~BLACK;

    return;
}

//
// Introduce a dispatch edge from an infinite loop, if exists, to the
// unwind node, so that the graph is connected from entry to exit.
//
void JavaFlowGraphBuilder::resolveWhileTrue()
{
    MemoryManager mm( fg->getMaxNodeId(), "ControlFlowGraph::resolveWhileTrue" );

    StlVector<int8> state( mm, fg->getMaxNodeId() );

    //
    // Pass 1: Identify all the nodes that are reachable from exit node.
    //
    uint32 nodesAffected = 0;
    reverseDFS( state, fg->getExitNode(), &nodesAffected );

    //
    // Pass 2: Add dispatches for while(true){;} loops, if they exist.
    //
    if( nodesAffected < fg->getNodes().size() ){
        Edges edgesForSplicing(mm);
        forwardDFS( state, fg->getEntryNode(), edgesForSplicing );
        for (uint32 i=0; i < edgesForSplicing.size(); i++) {
            Edge* e= edgesForSplicing[i];
            Node* targetNode = e->getTargetNode();
            LabelInst* newLabel = irBuilder.createLabel();
            Node* node = fg->spliceBlockOnEdge(e, newLabel);
            irBuilder.genLabel(newLabel);

            Opnd* args[] = { irBuilder.genTauSafe(), irBuilder.genTauSafe() };
            Type* returnType = irBuilder.getTypeManager()->getVoidType();
            irBuilder.genJitHelperCall( PseudoCanThrow,
                                        returnType,
                                        sizeof(args) / sizeof(args[0]),
                                        args );

            ExceptionInfo* exceptionInfo =
                (CatchBlock*)((LabelInst*)targetNode->getFirstInst())->getState();
            Node* dispatch = NULL;

            if( exceptionInfo != NULL) {
                dispatch = exceptionInfo->getLabelInst()->getNode();
            } else {
                dispatch = fg->getUnwindNode();
    }

            fg->addEdge(node, dispatch);
        }
        if( Log::isEnabled() ){
            MethodDesc &methodDesc = irBuilder.getIRManager()->getMethodDesc();
            Log::out() << "PRINTING LOG: After resolveWhileTrue" << ::std::endl;
            FlowGraph::printHIR(Log::out(), *fg, methodDesc );
        }

    }
}

Node* JavaFlowGraphBuilder::createBlockNodeOrdered(LabelInst* label) {
    if (label == NULL) {
        label = irBuilder.getInstFactory()->makeLabel();
    }
    Node* node = fg->createBlockNode(label);
    fallThruNodes.push_back(node);
    return node;
}

Node* JavaFlowGraphBuilder::createBlockNodeAfter(Node* node, LabelInst* label) {
    NodeList::iterator it = std::find(fallThruNodes.begin(), fallThruNodes.end(), node);
    assert(it!=fallThruNodes.end());
    if (label == NULL) {
        label = irBuilder.getInstFactory()->makeLabel();
    }
    label->setState(((LabelInst*)node->getFirstInst())->getState());
    Node* newNode = fg->createBlockNode(label);
    fallThruNodes.insert(++it, newNode);
    return newNode;
}

Node* JavaFlowGraphBuilder::createDispatchNode() {
    Node* node = fg->createDispatchNode(irBuilder.getInstFactory()->makeLabel());
    fallThruNodes.push_back(node);
    return node;
}

void JavaFlowGraphBuilder::addEdge(Node* source, Node* target) {
    if  (source->findTargetEdge(target) == NULL) {
        fg->addEdge(source, target);
    }
}

//
// construct flow graph
//
void JavaFlowGraphBuilder::build() {
    //
    // create epilog, unwind, and exit
    //
    InstFactory* instFactory = irBuilder.getInstFactory();
    fg->setReturnNode(createBlockNodeOrdered());
    fg->setUnwindNode(createDispatchNode());
    fg->setExitNode(fg->createNode(Node::Kind_Exit, instFactory->makeLabel()));
    fg->addEdge(fg->getReturnNode(), fg->getExitNode());
    fg->addEdge(fg->getUnwindNode(), fg->getExitNode());
    //
    // second phase: construct edges
    //
    createCFGEdges();
    //
    // third phase: add dispatches for infinite loop(s)
    //
    resolveWhileTrue();

    eliminateUnnestedLoopsOnDispatch();

}


//
//   look for edge patterns like this:
//
//      D C
//      C S
//      S has >1 in-edges
//      D has >1 in-edges
//
//      S  -- start node (sycle header)
//      D  -- dispatch node
//      C  -- catch node
//
//   when found, give a warning (to be resolved later)
//
//   ------------------------------------------------------------------------
//
//   look for edge pattern like this:
//
//      S  ME
//      ME D
//      S  D
//      D  C
//      C  S
//      S has >1 in-edges
//      D has >2 in-edges
//
//      S  -- start node (sycle header)
//      D  -- dispatch node
//      C  -- catch node
//      ME -- node with last instruction: 'monitorexit'
//
//   when found,
//      for each in-node of D (except 1st) do
//         duplicate D -> D2, 
//         retarget the in-node to D2
//         for all nodes (Cx) following D
//           if catch node 
//             duplicate Cx -> Cy
//             add edge 'D2 Cy'
//             add edge 'Cy succ(Cx)' // assertion failure if many succsessors
//           else
//             add edge 'D2 Cx'
//
void JavaFlowGraphBuilder::eliminateUnnestedLoopsOnDispatch()
{
    MemoryManager matched_nodes_mm(3*sizeof(Node*), "unnested_loops_mm");
    Nodes matched_dispatches(matched_nodes_mm);
    bool found_goto_into_loop_warning = false;
    const Nodes& nodes = fg->getNodes();
    Nodes::const_iterator niter;
    for (niter = nodes.begin(); niter != nodes.end(); ++niter) {
        Node* dispatch = *niter;
        if ( dispatch->isDispatchNode() && dispatch->getInDegree() > 1 ) {
            const Edges& out_edges =  dispatch->getOutEdges();
            for (Edges::const_iterator out_iter = out_edges.begin();
                out_iter != out_edges.end();
                out_iter++) {

                Node* catch_node = (*out_iter)->getTargetNode();
                if ( !catch_node->isCatchBlock() ) {
                    continue;
                }
                // TODO: Let catch edges contain only one instruction.
                //       In that case assertions will become meaningful.
                //assert(catch_node->hasOnlyOneSuccEdge());
                //assert(catch_node->hasOnlyOnePredEdge());

                Node* catch_target = (*(catch_node->getOutEdges().begin()))->getTargetNode();
                if ( catch_target->getInDegree() <= 1 ) {
                    continue;
                }
                bool found_monitorexit = false;
                bool dispatch_is_after_target = false;
                bool monitorexit_after_catch_target = false;
                const Edges& in_edges = dispatch->getInEdges();
                for (Edges::const_iterator in_iter = in_edges.begin();
                     in_iter != in_edges.end();
                     in_iter++) {
                    Node* pre_dispatch = (*in_iter)->getSourceNode();
                    if ( pre_dispatch == catch_target ) {
                        dispatch_is_after_target = true;
                    }
                    if ( lastInstIsMonitorExit(pre_dispatch) ) {
                        found_monitorexit = true;
                        if ( pre_dispatch->getInDegree() == 1 &&
                             (*pre_dispatch->getInEdges().begin())->getSourceNode() == catch_target ) {
                            monitorexit_after_catch_target = true;
                        }
                    }
                }

                if ( dispatch_is_after_target && 
                     found_monitorexit &&
                     monitorexit_after_catch_target ) {
                    if ( dispatch->getInDegree() == 2 ) {
                        //
                        // simple monitorexit code pattern, no goto into loop here
                        //
                        continue;
                    }else if ( dispatch->getInDegree() > 2 ) {
                        // goto into loop found
                        matched_dispatches.push_back(dispatch);
                        if ( Log::isEnabled() ) {
                            Log::out() << "goto into loop found, fixing..." << std::endl;
                        }
                        break;
                    }
                }
                if ( !found_goto_into_loop_warning ) {
                    if ( Log::isEnabled() ) {
                        Log::out() << "warning: maybe goto into loop with exception" 
                                   << std::endl;
                    }
                    found_goto_into_loop_warning = true;
                }
            }
        }
    }
    InstFactory* instFactory = irBuilder.getInstFactory();
    for ( Nodes::const_iterator dispatch_iter = matched_dispatches.begin();
          dispatch_iter != matched_dispatches.end();
          dispatch_iter++ ) {
        
        Node* dispatch = (*dispatch_iter);
        const Edges& in_edges = dispatch->getInEdges();
        assert(dispatch->getInDegree() > 1);
        while (in_edges.size() > 1) {
           Edge* in_edge = *(++in_edges.begin());
           Node* dup_dispatch = createDispatchNode();
           fg->replaceEdgeTarget(in_edge, dup_dispatch);
           const Edges& out_edges = dispatch->getOutEdges();
           for (Edges::const_iterator out_edge_iter = out_edges.begin(); 
                out_edge_iter != out_edges.end();
                out_edge_iter++) {
               Node* dispatch_target = (*out_edge_iter)->getTargetNode();
               if ( !dispatch_target->isCatchBlock() ) {
                   fg->addEdge(dup_dispatch, dispatch_target);
               }else{
                   CatchLabelInst* catch_label = (CatchLabelInst*)dispatch_target->getFirstInst();
                   Node* dup_catch = createBlockNodeOrdered(instFactory->makeCatchLabel(catch_label->getOrder(), catch_label->getExceptionType()));
                   fg->addEdge(dup_dispatch, dup_catch);
                   assert(dispatch_target->getOutDegree() == 1);
                   fg->addEdge(dup_catch, (*dispatch_target->getOutEdges().begin())->getTargetNode());
               }

           }
        }
    }
}

bool JavaFlowGraphBuilder::lastInstIsMonitorExit(Node* node)
{
    Inst* last = (Inst*)node->getLastInst();
    if ( last->getOpcode() == Op_TypeMonitorExit ||
         last->getOpcode() == Op_TauMonitorExit ) {
        return true;
    }
    return false;
}

JavaFlowGraphBuilder::JavaFlowGraphBuilder(MemoryManager& mm, IRBuilder &irB) : 
    memManager(mm), currentBlock(NULL), irBuilder(irB), fallThruNodes(mm)
{
    fg = irBuilder.getFlowGraph();
}

} //namespace Jitrino 
