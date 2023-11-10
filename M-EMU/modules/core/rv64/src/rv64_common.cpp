#include <climits>

#include "sdk/symbol_attr.h"

#include "module_manifest.hpp"
#include "RV64Core.h"

FuncReturnFeedback_e RV64Core::WriteProgramCounter_CoreAPI(RegItemVal_t reg_val)
{
    currentProgramCounter = reg_val.u64_val;
    return MEMU_OK;
}

FuncReturnFeedback_e RV64Core::SetGPRByIndex_CoreAPI(uint8_t gpr_index, int64_t val)
{
    assert(gpr_index < 32);
    if(unlikely(gpr_index == 0)) { return MEMU_OK; }
    GPR[gpr_index] = val;
    return MEMU_OK;
}



FuncReturnFeedback_e RV64Core::DumpProgramCounter_CoreAPI(RegisterItem_t &reg)
{
    reg.val.u64_val = currentProgramCounter;
    reg.reg_id = 33;
    reg.size = 8;
    strcpy(reg.disp_name, "pc");
    return MEMU_OK;
}

void RV64Core::reset() {
    trapProgramCounter = 0;
    needTrap = false;
    currentPrivMode = M_MODE;
    nextPrivMode = M_MODE;
    status = 0;
    CSReg_MStatus_t* mstatus = (CSReg_MStatus_t*)&status;
    mstatus->sxl = 2;
    mstatus->uxl = 2;
    csrMachineISA.ext = rv_ext('i') | rv_ext('m') | rv_ext('a') | rv_ext('c') | rv_ext('s') | rv_ext('u');
    csrMachineISA.mxl = 2;
    csrMachineISA.blank = 0;
    csrMExceptionDelegation = 0;
    csrMIntDelegation = 0;
    csrIntEnable = 0;
    csrMTrapVecBaseAddr.val = 0;
    csrMscratch = 0;
    csrMExceptionPC = 0;
    csrMachineCause.val = 0;
    csrMachineTrapVal = 0;
    csrMCounterEN.val = 0;
    csrIntPending.val = 0;
    csrMCycleNum = 0;
    minstret = 0;
    csrSTrapVecBaseAddr.val = 0;
    csrSscratch = 0;
    csrSExceptionPC = 0;
    csrSupervisorCause.val = 0;
    csrSupervisorTrapVal = 0;
    satp.val = 0;
    csrSCounterEN.val = 0;
    GPR[0] = 0;
    for(int i = 0; i < 32; i ++) GPR[i] = 0;
}










