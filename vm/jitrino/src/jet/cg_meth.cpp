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
 * @author Alexander Astapchuk
 * @version $Revision$
 */
/**
 * @file
 * @brief CodeGen's routines for method invocation and prolog/epilogue 
 *        generation.
 */
 
#include "compiler.h"
#include "trace.h"

#include <open/vm.h>
#include <jit_import.h>
#include <jit_intf.h>

#if !defined(_IPF_)
    #include "enc_ia32.h"
#endif

#include <algorithm>
using std::min;

namespace Jitrino {
namespace Jet {

/**
 * CallSig for monitor_enter and monitor_exit helpers.
 */
static CallSig cs_mon(CCONV_HELPERS, jobj);


void Compiler::gen_prolog(void) {
    if (is_set(DBG_TRACE_CG)) {
        dbg(";; ========================================================\n");
        dbg(";; Prolog: max_stack=%d, num_locals=%d, in_slots=%d\n", 
                m_stack.get_max_stack(), 
                m_stack.get_num_locals(),
                m_stack.get_in_slots());
        dbg(";; info_gc_stack_depth=%d, info_gc_locals=%d, info_gc_stack=%d",
                m_stack.info_gc_stack_depth(), 
                m_stack.info_gc_locals(),
                m_stack.info_gc_stack());
        dbg(";; stack_bot=%d, stack_max=%d\n",
                m_stack.stack_bot(), m_stack.stack_max());
        dbg(";; local(0)=%d\n", m_stack.local(0));
        dbg(";; native_stack_bot=%d\n", m_stack.unused());
        dbg(";; ========================================================\n");
    }

    unsigned prologStart = ipoff();
    //
    // Debugging things
    //

    // Ensure stack is aligned properly - for _ALIGN16 only here.
    // _ALIGN_HALF16 is handled below.
    if (is_set(DBG_CHECK_STACK) && (m_ci.cc() & CCONV_STACK_ALIGN16)){
        alu(alu_test, sp, 0xF);
        unsigned br_off = br(eq, 0, 0);
        gen_dbg_rt(false, "Misaligned stack @ %s", meth_fname());
        gen_brk();
        patch(br_off, ip());
    }
    
    if (is_set(DBG_BRK)) {
        gen_brk();
    }

    if (m_infoBlock.get_bc_size() == 1 && m_bc[0] == OPCODE_RETURN) {
        // empty method, nothing to do; the same is in gen_return();
        return;
    }
    
    // A special stack preparation is performed in order to deal with 
    // a stack overflow error (SOE) at runtime:
    // First, the callee-save registers are not changed until we are 
    // absolutely sure we have enouhg stack. In this case, if SOE happens,
    // we'll simply do nothing in unwind_stack().
    
    //
    // Allocate stack frame at the very beginning, so we are always in 
    // the predictable state in unwind_frame().
    //
    unsigned frameSize = m_stack.size();
    alu(alu_sub, sp, frameSize);
    
    // Ensure stack is aligned properly - do it here for _ALIGN_HALF16,
    // as the stack must be (sp%16==0) at this point
    if (is_set(DBG_CHECK_STACK) && (m_ci.cc() & CCONV_STACK_ALIGN_HALF16)){
        assert((frameSize+8) % 16 == 0);
        alu(alu_test, sp, 0xF);
        unsigned br_off = br(eq, 0, 0);
        gen_dbg_rt(false, "Misaligned stack @ %s", meth_fname());
        gen_brk();
        patch(br_off, ip());
    }
    
    // Lock all the args registers to avoid them to be rewritten by the 
    // frame setup procedures
    rlock(m_ci);
    
    // Here is pretty rare case, though still need to be proceeded:
    // When we allocate a stack frame of size more than one page then the 
    // memory page(s) may not be accessible and even not allocated.
    // A direct access to such [non existing] page raises 'access 
    // violation'. To avoid the problem we need simply probe (make read 
    // access) to the pages sequentially. In response on read-access to 
    // inaccessible page, the OS grows up the stack, so pages become 
    // accessible.
    const unsigned PAGE_SIZE = 0x1000;
    unsigned pages = 
                  (frameSize + m_max_native_stack_depth + 
                   PAGE_SIZE -1)/PAGE_SIZE;
    //
    for (unsigned i=1; i<pages; i++) {
        AR ar = valloc(i32);
        ld4(ar, sp, frameSize-i*PAGE_SIZE);
    }
    // When requested, store the whole context (==including scratch registers)
    // - normally for JVMTI PopFrame support.
    // Scratch registers get stored separately from the callee-save:
    // The callee-save registers are stored into spill area, but we can't
    // save scratch regs there - this area is already used to temporary
    // save scratch regs during method calls, etc (see gen_vm_call_restore).
    // Thus, we dedicate a separate place.
    const bool storeWholeContext =
        m_infoBlock.get_compile_params().exe_restore_context_after_unwind;
#ifdef _DEBUG
    // Fill the whole stack frame with a special value
    // -1 to avoid erasing retAddr
    int num_words = frameSize/sizeof(int) - 1; 
    AR ar = valloc(i32);
    Opnd fill(i32, ar);
    rlock(ar);
    AR ridx = valloc(i32);
    runlock(ar);
    Opnd idx(i32, ridx);
    //
    // When filling up the frame, the regs context is destroyed - preserve
    // it.
    if (storeWholeContext) {
        push(fill);
        push(idx);
    }
    //
    mov(fill, 0xDEADBEEF);
    mov(idx, num_words);
    unsigned _loop = ipoff();
    mov(Opnd(i32, sp, 0, ridx, 4), fill);
    alu(alu_sub, idx, 1);
    unsigned br_off = br(nz, 0, 0);
    patch(br_off, ip(_loop));
    if (storeWholeContext) {
        pop(idx);
        pop(fill);
    }
#endif

    // save callee-save registers. If frame size is less than 1 page, 
    // the page was not touched yet, and the SOE may happen here
    for (unsigned i=0; i<ar_num; i++) {
        AR ar = _ar(i);
        if (ar==sp || !is_callee_save(ar) || !m_global_rusage.test(i)) {
            continue;
        }
        // use maximum possible size to store the register
        jtype jt = is_f(ar) ? dbl64 : jobj;
        // Here, always use sp-based addressing - bp frame is not ready
        // yet.
        st(jt, ar, sp, frameSize+m_stack.spill(ar));
        m_infoBlock.saved(ar);
    }
    
    if (storeWholeContext) {
        // For JVMTI's PopFrame we store all scratch registers to a special
        // place.
        if (is_set(DBG_TRACE_CG)) { dbg(";;>jvmti.save.all.regs\n"); }
        for (unsigned i=0; i<ar_num; i++) {
            AR ar = _ar(i);
            if (is_callee_save(ar) || ar==sp) {
                continue;
            }
            // use maximum possible size to store the register
            jtype jt = is_f(ar) ? dbl64 : jobj;
            // Here, always use sp-based addressing - bp frame is not ready
            // yet.
            st(jt, ar, sp, frameSize+m_stack.jvmti_register_spill_offset(ar));
        }
        if (is_set(DBG_TRACE_CG)) { dbg(";;>~jvmti.save.all.regs\n"); }
    }


    // ok, if we pass to this point at runtime, then we have enough stack
    // and we stored all needed registers, so in case of unwind_stack() 
    // we'll simply restore registers from the stack.
    unsigned thisPoint = ipoff() - prologStart;
    m_infoBlock.set_warmup_len(thisPoint);
    if (m_base != sp) {
        // create bp-frame
        lea(m_base, Opnd(jobj, sp, frameSize));
    }
    
    // Must be here, after the stack get aligned
    if (is_set(DBG_TRACE_EE)) {
        gen_dbg_rt(true, "entering: %s", meth_fname());
    }

    //
    // reload input args into local vars
    //
    
    // an initial GC map for local variables which are copied from inputs
    ::std::vector<unsigned> locals_map;
    locals_map.resize(words(m_ci.count()));
    // an initial GC map for input args
    ::std::vector<unsigned> args_map;
    args_map.resize(words(m_ci.count()));
    // an initial GC map for callee-save registers
    unsigned regs_map = 0;
    
    // STACK_SLOT_SIZE <= retAddr
    unsigned const sp_offset = frameSize + STACK_SLOT_SIZE;
    
    // Spill out registers that are both input args and globally allocated
    for (unsigned i=0, local=0; i<m_ci.count(); i++, local++) {
        jtype jt = m_ci.jt(i);
        AR ar = m_ci.reg(i);
        if (ar != ar_x && m_global_rusage.test(ar_idx(ar))) {
            Opnd arg = m_ci.get(i);
            Val& var = vlocal(jt, local, true);
            mov(var.as_opnd(), arg);
            // A presumption, to simplify the code: if the managed calling
            // convention uses registers, then it's a platform without 'big'
            // type problem.
            assert(!is_big(jt));
        }
        if (is_wide(jt)) {
            ++local;
        }
    }

    // Now, process input args: 
    //  - set GC maps for objects came as input args, 
    //  - move input args into the slots in the local stack frame (for some
    //    args)
    for (unsigned i = 0, local=0; i<m_ci.count(); i++, local++) {
        jtype jt = m_ci.jt(i);
        // All values less than 32 bits get moved between methods as int32
        if (jt<i32) {
            jt = i32;
        }
        // If this is an object, then set a bit in appropriate map ...        
        if (jt == jobj) {
            AR ar = vreg(jobj, local);
            if (ar != ar_x && is_callee_save(ar)) {
                // .. callee-saved GP regs or ..
                regs_map |= 1<<ar_idx(m_ra[local]);
            }
            else if (vis_arg(local) || storeWholeContext) {
                // .. local vars that are kept on the input slots or
                // when we need to keep input args valid during enumeration
                // (for example for JVMTI PopFrame needs) ...
                assert(m_ci.reg(i) == ar_x || storeWholeContext);
                assert(0 == m_ci.off(i)%STACK_SLOT_SIZE);
                int inVal = m_ci.off(i)/STACK_SLOT_SIZE;
                // TODO: With storeWholeContext it only works with
                // stack-based parameters. On Intel64 with register-based
                // calling convention, need to track the registers that are
                // spilled separately from spill area (see above how the
                // storeWholeContext is processed).
                args_map[word_no(inVal)] =
                            args_map[word_no(inVal)] | (1 <<bit_no(inVal));
                if (storeWholeContext) {
                    // .. a 'regular' GC map for locals - must report
                    // together with input args in case of storeWholeContext
                    locals_map[word_no(local)] =
                                locals_map[word_no(local)] | (1 <<bit_no(local));
                }
            }
            else {
                // .. a 'regular' GC map for locals.
                locals_map[word_no(local)] = 
                            locals_map[word_no(local)] | (1 <<bit_no(local));
            }
        }
        jtype jtm = jtmov(jt);
        // as_type() => Convert narrow types (<i32) to i32.
        Opnd arg = m_ci.get(i, sp_offset).as_type(jt);

        // If we need to store 'this' for special reporting (i.e.
        // monitor_exit or for stack trace) - store it.
        if (i==0 && is_set(JMF_REPORT_THIS)) {
            if (is_set(DBG_TRACE_CG)) {dbg(";;>copy thiz\n");}
            assert(jt == jobj);
            Opnd thiz(jobj, m_base, voff(m_stack.thiz()));
            do_mov(thiz, arg);
            if (is_set(DBG_TRACE_CG)) {dbg(";;>~copy thizh\n");}
        }
        // If the local resides on the input arg, then no need to copy it 
        // from input arg into the frame.
        if (vis_arg(local)) {
            if (is_wide(jt)) {
                ++local;
            }
            continue;
        }
        //
        // Ok, copy the from input args area, into local variables area
        //
        
        // Define the slot, so it has proper type
        vvar_def(jt, local);
        if (arg.is_reg() && m_global_rusage.test(ar_idx(arg.reg()))) {
            // See a loop above - the argument already spilled into memory,
            // nothing to do
        }
        else {
            // forDef = true to avoid uploading, so it only returns memory 
            // operand
            Val& var = vlocal(jt, local, true);
            do_mov(var, arg);
            if (is_big(jt)) {
                // Presumption: on IA32 (<= is_big()==true) no i64 inputs 
                //              are left of input args
                assert(!vis_arg(local+1));
                // Presumption: on IA32 (<= is_big()==true) no i64 inputs 
                //              come on registers
                assert(arg.is_mem());
                assert(arg.index() == ar_x);
                Val arg_hi(jtm, arg.base(), arg.disp()+4);
                Val var_hi = vlocal(jt, local+1, true);
                do_mov(var_hi, arg_hi);
            }
        }
        if (is_wide(jt)) {
            ++local;
        }
    }
    
    runlock(m_ci);

    //
    // Store the GC map for the local variables that are initialized as 
    // they come from input args
    //
    if (is_set(DBG_TRACE_CG) && locals_map.size() != 0) {dbg(";;>locals.gc_map\n");}
    for (unsigned i = 0; i<locals_map.size(); i++) {
        Opnd map(i32, m_base, voff(m_stack.info_gc_locals()+i*sizeof(int)));
        Opnd val(locals_map[i]);
        mov(map, val);
    }
    //
    // For other local variables, zero the GC map
    //
    unsigned locals_gc_size = words(m_infoBlock.get_num_locals());
    if (locals_gc_size != locals_map.size()) {
        if (is_set(DBG_TRACE_CG)) {dbg(";;>locals.gc_map\n");}
        Opnd reg(i32, valloc(i32));
        alu(alu_xor, reg, reg);
        for (unsigned i=locals_map.size(); i<locals_gc_size; i++) {
            st4(reg.reg(), m_base, voff(m_stack.info_gc_locals()+i*sizeof(int)));
        }
    }
    //
    // Store the GC map for input args
    //
    if (is_set(DBG_TRACE_CG) && args_map.size() != 0) {dbg(";;>args.gc_map\n");}
    for (unsigned i = 0; i<args_map.size(); i++) {
        Opnd map(i32, m_base, voff(m_stack.info_gc_args()+i*sizeof(int)));
        Opnd val(args_map[i]);
        mov(map, val);
    }
    //
    // Store info about objects on registers
    //
    if (is_set(DBG_TRACE_CG)) {dbg(";;>regs.gc_map\n");}
    Opnd map(i32,  m_base, voff(m_stack.info_gc_regs()));
    Opnd val(regs_map);
    mov(map, val);
    //
    // Initial stack size is zero
    //
    if (is_set(DBG_TRACE_CG)) {dbg(";;>gc.stack_depth\n");}
    Opnd dpth(i32,  m_base, voff(m_stack.info_gc_stack_depth()));
    mov(dpth, Opnd(0));
    m_bbstate->stack_depth = 0;

    // Make the variables on their places - in a case if call to 
    // JVMTI/monitor_enter/recompilation helper lead to GC
    
    // TODO: May optimize a bit by specifying (0) - if the 0th BB is 
    // ref_count==1. In this case there is no real need to upload all the 
    // items on their registers. This will require special processing in both
    // bb_enter() and bb_leave()
    gen_bb_leave(NOTHING);
    
    //
    // now, everything is ready, may call VM/whatever
    //
    
    // Debugging - print out 'Entering ...'
    if (is_set(DBG_TRACE_EE)) {
        if (is_set(DBG_TRACE_CG)) {dbg(";;>print.ee\n");}
        rlock(cs_trace_arg);
        // Print out input args
        for (unsigned i=0, local=0; i<m_ci.count(); i++, local++) {
            // prepare stack
            if(cs_trace_arg.size() != 0) {
                alu(alu_sub, sp, cs_trace_arg.size());
            }
            // 'local'-th argument as a first arg for dbg_trace_arg() ...
            jtype jt = m_ci.jt(i);
            if (jt<i32) jt = i32;
            
            Opnd arg = cs_trace_arg.get(0);
            Val var;
            if (vreg(jt, local) != ar_x) {
                AR ar = vreg(jt, local);
                if (is_f(ar) && arg.is_reg()) {
                    // If the local var resides on a float-point register,
                    // and calling canovention uses registers to pass args
                    // - we can not simply do 'mov gr, fr'. Store fr to 
                    // memory first, then reload it to gr
                    assert(is_gr(arg.reg()));
                    Opnd scratch(jt, m_base, voff(m_stack.scratch()));
                    mov(scratch, Opnd(jt, ar));
                    
                    jt = jt = flt32 ? i32 : i64;
                    var = scratch.as_type(jt);
                }
                else {
                    var = Val(jt, ar);
                }
            }
            else {
                var = Val(jt, m_base, vlocal_off(local));
            }
            do_mov(arg, var.as_opnd(arg.jt()));
            // ... its type and index ...
            gen_call_novm(cs_trace_arg, (void*)&dbg_trace_arg, 1, i, jt);
            if (is_wide(jt)) {
                ++local;
            }
        }
        runlock(cs_trace_arg);
        if (is_set(DBG_TRACE_CG)) {dbg(";;>~print.ee\n");}
    }
    
    //
    // Profiling/recompilation support
    //
    
    if (is_set(JMF_PROF_ENTRY_BE)) {
        if (is_set(DBG_TRACE_CG)) { dbg(";;>profiling\n"); }
        // Increment entry counter
        AR ar = valloc(jobj);
        movp(ar, m_p_methentry_counter);
        Opnd addr(i32, ar, 0);
        if (is_set(JMF_PROF_SYNC_CHECK)) {
            rlock(ar);
            AR gr_val = valloc(i32);
            runlock(ar);
            
            Opnd val(i32, gr_val);
            Opnd thr(m_methentry_threshold);
            /* mov vreg, [counter] */ mov(val, addr);
            /* add vreg, 1         */ alu(alu_add, val, Opnd(1));
            /* mov [counter], vreg */ mov(addr, val);
            /* cmp vreg, threshold */ alu(alu_cmp, val, thr);
            /* jne keep_going      */ 
            /*      call recompile */ 
            /* keep_going:   ...   */ 
            unsigned br_off = br(ne, 0, 0, taken);
            gen_call_vm_restore(false, ci_helper_o, m_recomp_handler_ptr, 
                                0, m_profile_handle);
            patch(br_off, ip());
        }
        else {
            alu(alu_add, addr, Opnd(1));
        }
        if (is_set(DBG_TRACE_CG)) { dbg(";;>~profiling\n"); }
    }
    
    //
    // JVMTI method_enter notification
    //
    if (compilation_params.exe_notify_method_entry) {
        static const CallSig cs_ti_menter(CCONV_HELPERS, jobj);
        gen_call_vm(cs_ti_menter, rt_helper_ti_method_enter, 0, m_method);
    }
    
    
    if (meth_is_sync()) {
        if (is_set(DBG_TRACE_CG)) { dbg(";;>monitor_enter\n"); }
        if (method_is_static(m_method)) {
            gen_call_vm(cs_mon, rt_helper_monitor_enter_static, 0, m_klass);
        }
        else {
            AR gr = gr0;
            if (cs_mon.reg(0) != gr_x) {
                ld(jobj, cs_mon.reg(0), m_base, voff(m_stack.thiz()));
            }
            else {
                assert(cs_mon.size() != 0);
                alu(alu_sub, sp, cs_mon.size());
                ld(jobj, gr, m_base, voff(m_stack.thiz()));
                st(jobj, gr, sp, cs_mon.off(0));
            }
            gen_call_vm(cs_mon, rt_helper_monitor_enter, 1);
        }
        if (is_set(DBG_TRACE_CG)) { dbg(";;>~monitor_enter\n"); }
    }
    
    if (is_set(DBG_TRACE_CG)) {
        dbg_dump_state("after prolog", m_bbstate);
    }
}

void Compiler::gen_return(jtype retType)
{
    if (is_set(DBG_TRACE_EE)) {
        gen_dbg_rt(true, "exiting : %s", meth_fname());
    }
 
    if (m_infoBlock.get_bc_size() == 1 && m_bc[0] == OPCODE_RETURN) {
        // empty method, nothing to do; the same is in gen_prolog();
        // TODO: need to check and make sure whether it's absolutely legal
        // to bypass monitors on such an empty method
        // FIXME: this op9n bypasses JVMTI notifications
        ret(m_ci.caller_pops() ? 0 : m_ci.size());
        if (retType != jvoid) {
            vpop();
        }
        return;
    }
    
    bool is_sync = meth_is_sync();
    if (is_sync && meth_is_static()) {
        if (is_set(DBG_TRACE_CG)) {
            dbg(";;>monitor_exit\n");
        }
        gen_call_vm(cs_mon, rt_helper_monitor_exit_static, 0, m_klass);
        if (is_set(DBG_TRACE_CG)) {
            dbg(";;>~monitor_exit\n");
        }
    }
    else if (is_sync) {
        if (is_set(DBG_TRACE_CG)) {
            dbg(";;>monitor_exit\n");
        }
        AR gr = valloc(jobj);
        if (cs_mon.reg(0) != gr_x) {
            vpark(cs_mon.reg(0));
            ld(jobj, cs_mon.reg(0), m_base, voff(m_stack.thiz()));
        }
        else {
            assert(cs_mon.size() != 0);
            alu(alu_sub, sp, cs_mon.size());
            ld(jobj, gr, m_base, voff(m_stack.thiz()));
            st(jobj, gr, sp, cs_mon.off(0));
        }
        gen_call_vm(cs_mon, rt_helper_monitor_exit, 1);
        if (is_set(DBG_TRACE_CG)) {
            dbg(";;>~monitor_exit\n");
        }
    }
    
    if (compilation_params.exe_notify_method_exit) {
        // JVMTI helper takes pointer to return value and method handle
        const CallSig cs_ti_mexit(CCONV_STDCALL, jobj, jobj);
        // The call is a bit unusual, and is processed as follows:
        // we load an address of the top of the operand stack into 
        // a temporary register, and then pass this value as pointer
        // to return value. If method returns void, then we load address
        //of top of the stack anyway.
        Val retVal;
        rlock(cs_ti_mexit);
        Val retValPtr = Val(jobj, valloc(jobj));
        rlock(retValPtr);
        if (retType != jvoid) {
            // Make sure the top item is on the memory
            vswap(0);
            if (is_big(retType)) {
                vswap(1);
            }
            const Val& s = vstack(0);
            assert(s.is_mem());
            lea(retValPtr.as_opnd(), s.as_opnd());
        }
        else {
            Opnd stackTop(jobj, m_base, voff(m_stack.unused()));
            lea(retValPtr.as_opnd(), stackTop);
        }
        runlock(retValPtr);
        Val vmeth(jobj, m_method);
        gen_args(cs_ti_mexit, 0, &vmeth, &retValPtr);
        gen_call_vm(cs_ti_mexit, rt_helper_ti_method_exit, cs_ti_mexit.count());
        runlock(cs_ti_mexit);
    }

    if (is_f(retType)) {
#ifdef _IA32_
        // On IA-32 always swap to memory first, then upload into FPU
        vswap(0);
        ld(retType, fr_ret, m_base, vstack_off(0));
#else
        // Make sure the item is not immediate
        Val op = vstack(0, vis_imm(0));
        if (!op.is_reg() || op.reg() != fr_ret) {
            Opnd ret(retType, fr_ret);
            mov(ret, op.as_opnd());
        }
#endif
    }
    else if (is_big(retType)) {
#ifdef _IA32_
        vswap(0);
        vswap(1);
        ld4(eax.reg(), m_base, vstack_off(0));
        ld4(edx.reg(), m_base, vstack_off(1));
#else
        assert(false && "Unexpected case - 'big' type on EM64T");
#endif
        
    }
    else if (retType != jvoid) {
        Val& op = vstack(0);
        if (!op.is_reg() || op.reg() != gr_ret) {
            Opnd ret(retType, gr_ret);
            mov(ret, op.as_opnd());
        }
    }
    
    if (retType != jvoid && is_set(DBG_TRACE_EE)) {
        //TODO: the same code is in gen_save_ret() - extract into a 
        // separate method ?
        push_all();
        AR gtmp = gr0;
        Opnd op = vstack(0, true).as_opnd();
        st(jtmov(retType), op.reg(), m_base, voff(m_stack.scratch()));
        ld(jobj, gtmp, m_base, voff(m_stack.scratch()));
        if (cs_trace_arg.reg(0) != gr_x)  { 
            assert(cs_trace_arg.size() == 0);
            mov(cs_trace_arg.reg(0), gtmp);
        }
        else {
            assert(cs_trace_arg.size() != 0);
            alu(alu_sub, sp, cs_trace_arg.size());
            st4(gtmp, sp, cs_trace_arg.off(0));
        }
        Encoder::gen_args(cs_trace_arg, gtmp, 1, 2, -1, retType);
        movp(gtmp, (void*)&dbg_trace_arg);
        call(gtmp, cs_trace_arg, is_set(DBG_CHECK_STACK));
        pop_all();
    }

    unsigned frameSize = m_stack.size();
    // Restore callee-save regs
    for (unsigned i=0; i<ar_num; i++) {
        AR ar = _ar(i);
        if (ar==sp || !is_callee_save(ar) || !m_global_rusage.test(i)) {
            continue;
        }
        jtype jt = is_f(ar) ? dbl64 : jobj;
        // Here, always use sp-based addressing - bp frame may be destroyed
        // already by restoring bp.
        ld(jt, ar, sp, frameSize+m_stack.spill(ar));
    }
    
    alu(alu_add, sp, frameSize);
    ret(m_ci.caller_pops() ? 0 : m_ci.size());
    //m_jframe->clear_stack();
    if (retType != jvoid) {
        // free up registers
        vpop();
    }
}


void CodeGen::gen_invoke(JavaByteCodes opcod, Method_Handle meth,
                         const ::std::vector<jtype> &args, jtype retType)
{
    const unsigned slots = count_slots(args);
    // where (stack depth) 'this' is stored for the method being invoked 
    // (if applicable)
    const unsigned thiz_depth = slots - 1;

    const JInst& jinst = *m_curr_inst;
    
    CallSig cs(CCONV_MANAGED, args);
    for (unsigned i=0; i<cs.count(); i++) {
        AR ar = cs.reg(i);
        if (ar == ar_x) continue;
        vpark(ar);
    }
    unsigned stackFix = 0;
    rlock(cs);
    
    const bool is_static = opcod == OPCODE_INVOKESTATIC;
    Val thiz = is_static ? Val() : vstack(thiz_depth, true);
    if (meth == NULL) {
        runlock(cs); // was just locked above - unlock
        gen_call_throw(ci_helper_linkerr, rt_helper_throw_linking_exc, 0,
                       m_klass, jinst.op0, jinst.opcode);
        stackFix = gen_stack_to_args(true, cs, 0); // pop out args
        runlock(cs); // due to gen_stack_to_args()
        gen_gc_stack(-1, true);
        if (retType != jvoid) {
            gen_save_ret(retType);
        }
        if (stackFix != 0) {
            alu(alu_sub, sp, stackFix);
        }
        return;
    }
    
    if (!is_static) {
        rlock(thiz);
    }
    
    // INVOKEINTERFACE must have VM helper call first, so will place 
    // its args later, after the call, to avoid destruction of args 
    // on registers.
    if (opcod != OPCODE_INVOKEINTERFACE) {
        stackFix = gen_stack_to_args(true, cs, 0);
        gen_gc_stack(-1, true);
        vpark();
    }
    //
        // Check for null here - we just spilled all the args and 
        // parked all the registers, so we have a chance to use HW NPE 
    // For INVOKEINTERFACE we did not spill args, but we'll call VM first,
    // which is pretty expensive by itself, so the HW check does not give 
    // much.
    //
        if (!is_static) {
            // For invokeSPECIAL, we're using indirect address provided by 
            // the VM. This means we do not read vtable, which means no 
            // memory access, so we can't use HW checks - have to use 
        // explicit one. Not a big loss, as the INVOKESPECIAL mostly
            // comes right after NEW which guarantees non-null.
            gen_check_null(thiz, opcod != OPCODE_INVOKESPECIAL);
        }
    
    if (opcod == OPCODE_INVOKEINTERFACE) {
        // if it's INVOKEINTERFACE, then first resolve it
        Class_Handle klass = method_get_class(meth);
        const CallSig cs_vtbl(CCONV_STDCALL, jobj, jobj);
        // Prepare args for ldInterface helper
        if (cs_vtbl.reg(0) == gr_x) {
            assert(cs_vtbl.size() != 0);
            alu(alu_sub, sp, cs_vtbl.size());
            st(jobj, thiz.reg(), sp, cs_vtbl.off(0));
        }
        else {
            assert(cs_vtbl.size() == 0);
            mov(cs_vtbl.get(0), thiz.as_opnd());
        }
        gen_call_vm(cs_vtbl, rt_helper_get_vtable, 1, klass);
        //
        // Method's vtable is in gr_ret now, prepare stack
        //
        rlock(gr_ret);
        //st(jobj, gr_ret, m_base, voff(m_stack.scratch()));
        stackFix = gen_stack_to_args(true, cs, 0);
        gen_gc_stack(-1, true);
        vpark();
        unsigned offset = method_get_offset(meth);
        //ld(jobj, gr_ret, m_base, voff(m_stack.scratch()));
        runlock(gr_ret);
        ld(jobj, gr_ret, gr_ret, offset);
        call(gr_ret, cs, is_set(DBG_CHECK_STACK));
    }
    else if (opcod == OPCODE_INVOKEVIRTUAL) {
        AR gr = valloc(jobj);
        unsigned offset = method_get_offset(meth);
        Opnd ptr;
        if (g_vtbl_squeeze) {
            ld4(gr, thiz.reg(), rt_vtable_offset);
            AR gr_vtbase = valloc(jobj);
            movp(gr_vtbase, (char*)VTBL_BASE+offset);
            alu(jobj, alu_add, gr, gr_vtbase);
            ptr = Opnd(jobj, gr, 0);
        }
        else {
            ld(jobj, gr, thiz.reg(), rt_vtable_offset);
            ptr = Opnd(jobj, gr, offset);
        }
        call(ptr, cs, is_set(DBG_CHECK_STACK));
    }
    else {
        void * paddr = method_get_indirect_address(meth);
#ifdef _IA32_
        Opnd ptr(jobj, ar_x, paddr);
#else
        AR gr = valloc(jobj);
        movp(gr, paddr);
        ld(jobj, gr, gr);
        Opnd ptr(jobj, gr);
#endif
        call(ptr, cs, is_set(DBG_CHECK_STACK));
    }
    
    if (!is_static) {
        runlock(thiz);
    }
    // to unlock after gen_stack_to_args()
    runlock(cs);
    // to unlock after explicit lock at the top of this method
    runlock(cs);
    
    if (retType != jvoid) {
        gen_save_ret(retType);
    }
    if (stackFix != 0) {
        alu(alu_sub, sp, stackFix);
    }
}

void CodeGen::gen_args(const CallSig& cs, unsigned idx, const Val * parg0, 
                       const Val * parg1, const Val * parg2, const Val * parg3, 
                       const Val * parg4, const Val * parg5, const Val * parg6)
{
    if (idx == 0 && cs.size() != 0) {
        alu(alu_sub, sp, cs.size());
    }
    
    const Val* args[] = {parg0, parg1, parg2, parg3, parg4, parg5, parg6};
    unsigned steps = min((int)COUNTOF(args), (int)cs.count()-(int)idx);
    for (unsigned i=0; i<steps; i++) {
        if (args[i] == 0) {
            break;
        }
        unsigned id = idx + i;
        Opnd arg = cs.get(id);
        do_mov(arg, *args[i]);
    }
}

void CodeGen::gen_save_ret(jtype jt)
{
    assert(jt != jvoid);
    AR ar = is_f(jt) ? fr_ret : gr_ret;
    if (jt==i8) {
        sx1(Opnd(i32, ar), Opnd(jt,ar));
        jt = i32;
    }
    else if (jt == i16) {
        sx2(Opnd(i32, ar), Opnd(jt,ar));
        jt = i32;
    }
    else if (jt == u16) {
        zx2(Opnd(i32, ar), Opnd(jt,ar));
        jt = i32;
    }
#ifdef _IA32_
    if(ar == fr_ret) {
        // Cant use vstack_off right here, as the item is not yet pushed.
        unsigned slot = m_jframe->size();
        if (is_wide(jt)) {
            slot += 1;
        }
        vpush(Val(jt, m_base, voff(m_stack.stack_slot(slot))));
        //
        st(jt, fr_ret, m_base, vstack_off(0));
    }
    else if (is_big(jt)) {
        assert(jt==i64);
        static const AR eax = virt(RegName_EAX);
        static const AR edx = virt(RegName_EDX);
        vpush2(Val(jt, eax), Val(jt, edx));
    }
    else
#endif
    {
        assert(!is_big(jt));
        vpush(Val(jt, ar));
    }

    if (is_set(DBG_TRACE_EE) && !is_f(jt) && !is_big(jt)) {
        push_all(true);
        assert(!is_callee_save(gr0));
        AR gtmp = gr0;
        //ld(jobj, gtmp, bp, m_stack.stack_slot(m_jframe->depth2slot(0)));
        Opnd tmp(jt, gtmp);
        mov(tmp, Opnd(jt, gr_ret));
        if (cs_trace_arg.reg(0) != gr_x)  { 
            assert(cs_trace_arg.size() == 0);
            mov(cs_trace_arg.reg(0), gtmp);
        }
        else {
            assert(cs_trace_arg.size() != 0);
            alu(alu_sub, sp, cs_trace_arg.size());
            mov(Opnd(jt, sp, cs_trace_arg.off(0)), tmp);
        }
        Encoder::gen_args(cs_trace_arg, gtmp, 1, 2, -1, jt);
        movp(gtmp, (void*)&dbg_trace_arg);
        call(gtmp, cs_trace_arg, is_set(DBG_CHECK_STACK));
        pop_all();
    }

}

}}; // ~namespace Jitrino::Jet
