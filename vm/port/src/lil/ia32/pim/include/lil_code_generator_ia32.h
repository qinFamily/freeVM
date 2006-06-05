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
 * @author Intel, Evgueni Brevnov, Ivan Volosyuk
 * @version $Revision: 1.1.2.1.4.4 $
 */  


#ifndef _LIL_CODE_GENERATOR_IA32_
#define _LIL_CODE_GENERATOR_IA32_

#include "lil.h"
#include "lil_code_generator.h"

class LilCodeGeneratorIa32 : public LilCodeGenerator {

 public:
    LilCodeGeneratorIa32();

 protected:
    NativeCodePtr compile_main(LilCodeStub* , size_t*, const char*, bool);
};

#endif // _LIL_CODE_GENERATOR_IA32_
