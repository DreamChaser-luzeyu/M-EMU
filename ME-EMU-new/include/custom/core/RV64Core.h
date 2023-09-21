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

    // ----- CSRs
    // --- M_MODE CSRs
    uint64_t                csrMCycleNum;
    CSReg_IntPending_t      csrMIntPending;            // mip, Read-only for user
    uint64_t                csrMIntEnable;             // mie, TODO: mask?
    // Used as a mask, indicating which traps to be routed to S_MODE
    uint64_t                csrMIntDelegation;         // mideleg, Machine-level Interrupt Delegation Register
    uint64_t                csrMExceptionDelegation;   // medeleg, Machine-level Exception Delegation Register
    uint64_t                csrMachineTrapVal;         // mtval,   Machine Trap Value Register
    CSReg_MStatus_t         csrMachineStatus;
    CSReg_Cause_t           csrMachineCause;
    uint64_t                csrMExceptionPC;           // mepc
    CSReg_TrapVector_t      csrMTrapVecBaseAddr;       // mtvec
    // --- S_MODE CSRs
    CSReg_SStatus_t         csrSupervisorStatus;
    uint64_t                csrSupervisorTrapVal;      // stval
    CSReg_Cause_t           csrSupervisorCause;        // scause
    uint64_t                csrSExceptionPC;           // sepc
    CSReg_TrapVector_t      csrSTrapVecBaseAddr;       // stvec
    // --- Shared CSRs
    uint64_t                mcycle;
    uint64_t                minstret;
    // --- Inner states
    bool needTrap;
    uint64_t trapProgramCounter;
    PrivMode_t currentPrivMode;
    PrivMode_t nextPrivMode;
    uint64_t currentProgramCounter;


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
    void preExec();
    void raiseTrap(CSReg_Cause_t cause, uint64_t tval = 0);
};



