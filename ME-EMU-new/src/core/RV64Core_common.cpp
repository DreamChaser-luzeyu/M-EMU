#include "custom/core/RV64Core.h"


FuncReturnFeedback_t RV64Core::DumpRegister_CoreAPI(std::vector<RegisterItem_t> &regs)
{
    return MEMU_OK;
}

FuncReturnFeedback_t RV64Core::Step_CoreAPI() {
    bool is_new_pc_set = false;
    bool is_compressed_inst = false;
    bool is_instr_illegal = false;
    // --- Instruction Fetch
    RVInst_t instruction;

    // --- Decode & exec
    preExec();
    if(needTrap) {}



    currentProgramCounter = currentProgramCounter + (is_compressed_inst ? 2 : 4);
exception:
    if(is_instr_illegal) {
        CSReg_Cause_t cause;
        cause.cause = exc_illegal_instr;
        raiseTrap(cause, instruction.val);
    }
    if(needTrap) { currentProgramCounter = trapProgramCounter; }
post_exec:
    if(!needTrap) { minstret++; }
    // seems useless, to be removed
//    assert(csrMachineStatus.blank0 == 0);
//    assert(csrMachineStatus.blank1 == 0);
//    assert(csrMachineStatus.blank2 == 0);
//    assert(csrMachineStatus.blank3 == 0);
//    assert(csrMachineStatus.blank4 == 0);
    return MEMU_OK;
}

FuncReturnFeedback_t RV64Core::DumpProgramCounter_CoreAPI(RegisterItem_t &reg)
{
    return MEMU_OK;
}

FuncReturnFeedback_t RV64Core::WriteProgramCounter_CoreAPI(RegItemVal_t reg_val)
{
    return MEMU_OK;
}

FuncReturnFeedback_t RV64Core::setRegByIndex_CoreAPI(uint8_t gpr_index, int64_t val)
{
    return MEMU_OK;
}


