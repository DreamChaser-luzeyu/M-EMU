#include <limits.h>
#include "module_manifest.hpp"
#include "RV64Core.h"


FuncReturnFeedback_e RV64Core::DumpRegister_CoreAPI(std::vector<RegisterItem_t> &regs)
{
    return MEMU_OK;
}

FuncReturnFeedback_e RV64Core::DumpProgramCounter_CoreAPI(RegisterItem_t &reg)
{
    return MEMU_OK;
}

FuncReturnFeedback_e RV64Core::WriteProgramCounter_CoreAPI(RegItemVal_t reg_val)
{
    currentProgramCounter = reg_val.u64_val;
    return MEMU_OK;
}

FuncReturnFeedback_e RV64Core::setGPRByIndex_CoreAPI(uint8_t gpr_index, int64_t val)
{
    assert(gpr_index < 32);
    if(gpr_index == 0) { return MEMU_OK; }
    GPR[gpr_index] = val;
    return MEMU_OK;
}


int64_t RV64Core::alu_exec(int64_t a, int64_t b, ALU_Op_enum op, bool op_32) {
    if (op_32) {
        a = (int32_t)a;
        b = (int32_t)b;
    }
    int64_t result = 0;
    switch (op) {
        case ALU_ADD:
            result = a + b;
            break;
        case ALU_SUB:
            result = a - b;
            break;
        case ALU_SLL:
            result = a << (b & (op_32 ? 0x1f : 0x3f));
            break;
        case ALU_SLT:
            result = a < b;
            break;
        case ALU_SLTU:
            result = (uint64_t)a < (uint64_t)b;
            break;
        case ALU_XOR:
            result = a ^ b;
            break;
        case ALU_SRL:
            result = (uint64_t)(op_32 ? (a&0xffffffff) : a) >> (b & (op_32 ? 0x1f : 0x3f));
            break;
        case ALU_SRA:
            result = a >> (b & (op_32 ? 0x1f : 0x3f));
            break;
        case ALU_OR:
            result = a | b;
            break;
        case ALU_AND:
            result = a & b;
            break;
        case ALU_MUL:
            result = a * b;
            break;
        case ALU_MULH:
            result = ((__int128_t)a * (__int128_t)b) >> 64;
            break;
        case ALU_MULHU:
            result = (static_cast<__uint128_t>(static_cast<uint64_t>(a))*static_cast<__uint128_t>(static_cast<uint64_t>(b))) >> 64;
            break;
        case ALU_MULHSU:
            result = (static_cast<__int128_t>(a)*static_cast<__uint128_t>(static_cast<uint64_t>(b))) >> 64;
            break;
        case ALU_DIV:
            if (b == 0) result = -1;
            else if (a == LONG_MIN && b == -1) result = LONG_MIN;
            else result = a / b;
            break;
        case ALU_DIVU:
            if (b == 0) result = ULONG_MAX;
            else if (op_32) result = (uint32_t)a / (uint32_t)b;
            else result = ((uint64_t)a) / ((uint64_t)b);
            break;
        case ALU_REM:
            if (b == 0) result = a;
            else if (a == LONG_MIN && b == -1) result = 0;
            else result = a % b;
            break;
        case ALU_REMU:
            if (b == 0) result = a;
            else if (op_32) result = (uint32_t)a % (uint32_t)b;
            else result = (uint64_t)a % (uint64_t)b;
            break;
        default:
            assert(false);
    }
    if (op_32) result = (int32_t)result;
    return result;
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

    for(int i=0; i<32; i++) setGPR(i, 0);
}










