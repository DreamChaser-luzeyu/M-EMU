#include "custom/core/RV64Core.h"


FuncReturnFeedback_t RV64Core::DumpRegister_CoreAPI(std::vector<RegisterItem_t> &regs)
{
    return MEMU_OK;
}

FuncReturnFeedback_t RV64Core::Step_CoreAPI() {
    // --- Instruction Fetch
    RVInst_t instruction;

    // --- Decode & exec




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
