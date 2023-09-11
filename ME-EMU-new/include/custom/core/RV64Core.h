#pragma once

#include "interface/Core.h"
#include "interface/MMIO_Bus.h"
#include "RV64_structs.h"

struct IntStatus {
    bool meip;
    bool msip;
    bool mtip;
    bool seip;
};
typedef IntStatus IntStatus_t;

class RV64Core : public ProcessorCore_I {
    // ----- Fields
private:
    MMIOBus_I* sysBus;
    uint16_t hartID;

    // --- CSRs
    uint64_t                csrMCycleNum;
    CSReg_IntPending_t      csrMIntPending;      // Read-only for user
    uint64_t                csrMIntEnable;       // TODO: mask?
    uint64_t                csrMIntDelegation;   // Machine-level Interrupt Delegation Register
    CSReg_MachineStatus_t   csrMachineStatus;
    // --- Inner states
    bool needTrap;
    PrivMode_t currentPrivMode;
    PrivMode_t nextPrivMode;

    // ----- Interface implementations
public:
    FuncReturnFeedback_t DumpRegister_CoreAPI(std::vector<RegisterItem_t>& regs) override;
    FuncReturnFeedback_t Step_CoreAPI() override;
    FuncReturnFeedback_t DumpProgramCounter_CoreAPI(RegisterItem_t& reg) override;
    FuncReturnFeedback_t WriteProgramCounter_CoreAPI(RegItemVal_t reg_val) override;
    FuncReturnFeedback_t setRegByIndex_CoreAPI(uint8_t gpr_index, int64_t val) override;


    // ----- Constructors & Destructors
    RV64Core(MMIOBus_I* sys_bus, uint16_t hart_id) :
        sysBus(sys_bus), hartID(hart_id), csrMCycleNum(0), needTrap(false) {
        this->intStatus = new IntStatus_t;
        intStatus->mtip = false;
        intStatus->msip = false;
        intStatus->meip = false;
        intStatus->seip = false;
    }

    // ----- Member functions
private:
    IntType_e preExec();
};



