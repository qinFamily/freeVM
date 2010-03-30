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
 * @author Intel, Pavel A. Ozhdikhin
 *
 */

#ifndef _TAIL_DUPLICATOR_H_
#define _TAIL_DUPLICATOR_H_

#include "optpass.h"

namespace Jitrino {

class IRManager;
class DominatorTree;
class DominatorNode;
class LoopTree;
class DefUseBuilder;
class Node;
class BranchInst;


class TailDuplicator {
public:
    TailDuplicator(IRManager& irm, DominatorTree* dtree) : _irm(irm), _dtree(dtree) {}

    void doTailDuplication();
    void doProfileGuidedTailDuplication(LoopTree* ltree);

private:
    bool isMatchingBranch(BranchInst* br1, BranchInst* br2);
    void process(DefUseBuilder& defUses, DominatorNode* dnode);
    void tailDuplicate(DefUseBuilder& defUses, Node* idom, Node* tail);
    void profileGuidedTailDuplicate(LoopTree* ltree, DefUseBuilder& defUses, Node* node);

    IRManager& _irm;
    DominatorTree* _dtree;
};

} //namespace Jitrino 

#endif // _TAIL_DUPLICATOR_H_
