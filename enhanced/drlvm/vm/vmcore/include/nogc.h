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
 * @author Intel, Alexei Fedotov
 * @version $Revision: 1.1.2.1.4.3 $
 */  


// This file contains the prototypes for allocation routines that are not currently supported by the GC.
// The clients of these routines shouldn't be relying on gc_for_vm.h or the older garbage_collector.h. 
// Instead, this stop gap header file...
//

#ifndef _NOGC_H_
#define _NOGC_H_

#include "jit_import.h" // for the definition of Code_Allocation_Action

#ifdef _IPF_
#define DEFAULT_CODE_ALIGNMENT 32
#else // !_IPF_
#define DEFAULT_CODE_ALIGNMENT 16
#endif // !_IPF_

void vm_init_mem_alloc();
void *malloc_fixed_code_for_jit(size_t size, size_t alignment, unsigned heat, Code_Allocation_Action action);

// Allocate memory for vtable data.
void *allocate_vtable_data_from_pool(size_t size);

// deallocate memory when finished
void vm_mem_dealloc();

#endif // _NOGC_H_