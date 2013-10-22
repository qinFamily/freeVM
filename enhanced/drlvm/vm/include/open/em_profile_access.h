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
 * @author Mikhail Y. Fursov
 * @version $Revision: 1.1.2.2.4.4 $
 */
#ifndef _EM_PROFILE_ACCESS_H_
#define _EM_PROFILE_ACCESS_H_

#include "open/types.h"
#include "open/em.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Known profiler types. Each of the profilers
  * is represented with separate interface to 
  * create and to access to profiles.
  */
enum EM_PCTYPE {
    /**  Entry-backedge profiler
      *  Collects number execution counts for
      *  methods entries and backedges if present.
      */
    EM_PCTYPE_ENTRY_BACKEDGE=1,
    /**  Edge profiler.
      *  Collects profile for method entry and 
      *  all edges in IR Control Flow Graph
      */
    EM_PCTYPE_EDGE=2
};

/** A EM interface used to access to profile collectos*/
typedef struct EM_ProfileAccessInterface {

    /** Request profile collector typ for specified profile collector handle 
      *  @param  _this - EM instance profile collector belongs to
      *  @param  pc    - profile collector handle we interested in
      *  @return       - the type of profile collector
      */
    EM_PCTYPE               (*get_pc_type) (
                                EM_Handle _this,
                                PC_Handle pc
                            );

    /** Request method profile from profile collector
      * @param _this     - EM instance profile collector belongs to
      * @param pc        - profile collector used to collect profile
      * @param mh        - method we asking profile for
      * @return          - method profile handle, that can be used to access
      *                    to custom method profile properties with 
      *                    specialized profile collector interface
      */
    Method_Profile_Handle   (*get_method_profile)(
                                EM_Handle _this,
                                PC_Handle pc,
                                Method_Handle mh
                            );

    /** Request profile collector of specified type and role for a JIT
      * @param _this          - EM instance profile collector belongs to
      * @param profile_type   - the type of profile collector
      * @param jh             - handle to JIT, profile collector created for.
      * @param jit_role       - the role of JIT: the user or supplier of profile
      * @return               - the handle to profile collector instance 
      */

    PC_Handle               (*get_pc)(
                                EM_Handle _this,
                                EM_PCTYPE profile_type,
                                JIT_Handle jh,
                                EM_JIT_PC_Role jit_role
                            );



    // Here follows entry-backedge and edge profilers interfaces.
    // All methods below could be moved into separate EB and Edge 
    // profiler collectors specific files.

    /** Create new entry-backedge profile for a method
      * Only one profile per method can be created for a single 
      * profile collector instance 
      */
    Method_Profile_Handle (*eb_profiler_create_profile) (PC_Handle ph, Method_Handle mh);

    /** Request the address of entry counter
      * JIT configured to generate entry-backedge profile must 
      * emit the code to increment this counter every time a method called
      */
    void* (*eb_profiler_get_entry_counter_addr)(Method_Profile_Handle mph);

    /** Request the address of backedge counter
      * JIT configured to generate entry-backedge profile must 
      * emit the code to increment this counter every time any backedge in
      * a method is called
      */
    void* (*eb_profiler_get_backedge_counter_addr)(Method_Profile_Handle mph);

    /** Check if entry-backedge profiler is configured to work in synchronous mode
      * In synchronous mode JIT is responsible to emit checks that counter's limit
      * is reached for both entry and backedge counters. If limit is reached 
      * eb_profiler_sync_mode_callback must be called directly from managed code
      * In asynchronous mode counters checks are done by profile collector in a 
      * separate thread.
      * @see eb_profiler_sync_mode_callback()
      */
    char (*eb_profiler_is_in_sync_mode)(PC_Handle pch);

    /** If profile collector is in 'sync' mode (@see eb_profiler_is_in_sync_mode())
      * JIT must call this method every time the counter limit is reached
      */
    void (*eb_profiler_sync_mode_callback)(Method_Profile_Handle mph);

    /** Return the counter's limit for entry threshold for a given
      * profile collector
      */
    uint32 (*eb_profiler_get_entry_threshold)(PC_Handle pch);

    /** Return the counter's limit for backedge threshold for a given
      * profile collector
      */
    uint32 (*eb_profiler_get_backedge_threshold)(PC_Handle pch);



    //EDGE profiler interface

    /** Create an edge profile for a method. 
      * Only one profile per method can be created for a single 
      * profile collector instance 
      * @param ph               - edge profile collector handle
      * @param mh               - method handle to create profile for
      * @param numEdgeCounters  - number of edge counters in a method
      * @param counterKeys      - the keys, or numbers, will be associated with 
      *                           each counter. The key must be used to access to
      *                           counter value
      * @param checksum         - profile checksum
      * @return                 - a handle to access method profile data
      */
    Method_Profile_Handle (*edge_profiler_create_profile) (PC_Handle ph, Method_Handle mh, uint32 numEdgeCounters, uint32* counterKeys, uint32 checkSum);


    /** Return number of edge counters in profile */
    uint32 (*edge_profiler_get_num_counters)(Method_Profile_Handle mph);

    /** Return profile checksum*/
    uint32 (*edge_profiler_get_checksum)(Method_Profile_Handle mph);

    /** Return the address of counter assosiated with key*/
    void* (*edge_profiler_get_counter_addr)(Method_Profile_Handle mph, uint32 key);

    /** Return the address of entry counter*/
    void* (*edge_profiler_get_entry_counter_addr)(Method_Profile_Handle mph);

    /** Return the entry threshold for profile collector
      */
    uint32 (*edge_profiler_get_entry_threshold)(PC_Handle pch);
    
    /** Return the edge threshold for profile collector
      */
    uint32 (*edge_profiler_get_backedge_threshold)(PC_Handle pch);


} EM_ProfileAccessInterface;


#ifdef __cplusplus
}
#endif


#endif
