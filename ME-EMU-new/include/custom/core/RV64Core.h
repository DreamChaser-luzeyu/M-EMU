#pragma once

#include "interface/Core.h"
#include "interface/MMIO_Bus.h"
#include "RV64_structs.h"
#include "RV64_CSR_structs.h"
#include "interface/MMU.h"
#include "RV64SV39_MMU.h"
#include "custom/core/RV64_IntStatus.h"

#include <iostream>
#include <fstream>

//struct IntStatus {
//    bool meip;
//    bool msip;
//    bool mtip;
//    bool seip;
//};
//typedef IntStatus IntStatus_t;


enum ALU_Op_enum {
    ALU_ADD, ALU_SUB, ALU_SLL, ALU_SLT, ALU_SLTU, ALU_XOR, ALU_SRL, ALU_SRA, ALU_OR, ALU_AND,
    ALU_MUL, ALU_MULH, ALU_MULHU, ALU_MULHSU, ALU_DIV, ALU_DIVU, ALU_REM, ALU_REMU,
    ALU_NOP
};


static inline IntType_e bits_to_int_type(uint64_t int_bits) {
    // According to spec, multiple simultaneous interrupts destined for M-mode are handled
    // in the following sequence: MEI, MSI, MTI, SEI, SSI, STI.
    if (int_bits & (1ull<<int_m_ext)) {
        return int_m_ext;
    }
    else if (int_bits & (1ull<<int_m_sw)) {
        return int_m_sw;
    }
    else if (int_bits & (1ull<<int_m_timer)) {
        return int_m_timer;
    }
    else if (int_bits & (1ull<<int_s_ext)) {
        return int_s_ext;
    }
    else if (int_bits & (1ull<<int_s_sw)) {
        return int_s_sw;
    }
    else if (int_bits & (1ull<<int_s_timer)) {
        return int_s_timer;
    }
    return int_no_int;
}

class RV64Core : public ProcessorCore_I {
    // ----- Constants
    const uint64_t S_MODE_INT_MASK = (1ull << int_s_ext) | (1ull << int_s_sw) | (1ull << int_s_timer);
    const uint64_t COUNTER_MASK = (1 << 0) | (1 << 2);
    const uint64_t S_MODE_EXC_MASK = (1 << 16) - 1 - (1 << exec_ecall_from_machine);
    const uint64_t M_MODE_INT_MASK = S_MODE_INT_MASK | (1ull << int_m_ext) | (1ull << int_m_sw) | (1ull << int_m_timer);
    const uint8_t PC_ALIGN = 2;
    // ----- Fields
private:
    MMIOBus_I* sysBus;
    uint16_t hartID;
    RV64SV39_MMU* sv39MMU;

    // ----- Regs
    int64_t GPR[32];                                   // General purpose registers
    // ----- CSRs
    // --- M_MODE CSRs
    uint64_t                csrMCycleNum;
    // Used as a mask, indicating which traps to be routed to S_MODE
    uint64_t                csrMIntDelegation;         // mideleg, Machine-level Interrupt Delegation Register
    uint64_t                csrMExceptionDelegation;   // medeleg, Machine-level Exception Delegation Register
    uint64_t                csrMachineTrapVal;         // mtval,   Machine Trap Value Register
    CSReg_Cause_t           csrMachineCause;           // mcause
    uint64_t                csrMExceptionPC;           // mepc
    CSReg_TrapVector_t      csrMTrapVecBaseAddr;       // mtvec
    CSR_CounterEN_t         csrMCounterEN;             // mcounteren
    CSR_MISA_t              csrMachineISA;             // misa
    uint64_t                csrMscratch;               // mscratch
    // --- S_MODE CSRs
    uint64_t                csrSupervisorTrapVal;      // stval
    CSReg_Cause_t           csrSupervisorCause;        // scause
    uint64_t                csrSExceptionPC;           // sepc
    CSReg_TrapVector_t      csrSTrapVecBaseAddr;       // stvec
    CSR_CounterEN_t         csrSCounterEN;             // scounteren
    uint64_t                csrSscratch;               // sscratch
    // --- Shared CSRs
    uint64_t                mcycle;
    uint64_t                minstret;
    // The sstatus register is a subset of the mstatus register.
    // In a straightforward implementation, reading or writing any field in sstatus is equivalent to
    // reading or writing the homonymous field in mstatus.
    uint64_t                status;                    // mstatus, sstatus
    uint64_t                rvHartID;
    SATP_Reg_t              satp;
    // Restricted views of the mip and mie registers appear as the sip and sie registers for supervisor
    // level.
    CSReg_IntPending_t      csrIntPending;             // mip, sip
    uint64_t                csrIntEnable;              // mie, sie TODO: mask?
    uint64_t                trapProgramCounter;
    uint64_t                currentProgramCounter;

    // ----- Inner states
    bool needTrap;
    PrivMode_e              currentPrivMode;
    PrivMode_e              nextPrivMode;


    // ----- Interface implementations
public:
    FuncReturnFeedback_e DumpRegister_CoreAPI(std::vector<RegisterItem_t>& regs) override;
    FuncReturnFeedback_e Step_CoreAPI() override;
    FuncReturnFeedback_e DumpProgramCounter_CoreAPI(RegisterItem_t& reg) override;
    FuncReturnFeedback_e WriteProgramCounter_CoreAPI(RegItemVal_t reg_val) override;
    FuncReturnFeedback_e setRegByIndex_CoreAPI(uint8_t gpr_index, int64_t val) override;


    // ----- Constructors & Destructors
    RV64Core(MMIOBus_I* sys_bus, uint16_t hart_id) :
        sysBus(sys_bus), hartID(hart_id), csrMCycleNum(0), needTrap(false) {
        this->intStatus = new IntStatus_t;
        intStatus->mtip = false;
        intStatus->msip = false;
        intStatus->meip = false;
        intStatus->seip = false;
        this->sv39MMU = new RV64SV39_MMU(sys_bus, currentPrivMode, satp, status, rvHartID);
        this->rvHartID = hart_id;
        reset();
    }

    // ----- Member functions
private:
    void preExec();
//    void decodeExec(const RVInst_t& inst, bool& is_instr_illegal);

    /**
     * Do calculations
     * @param a operand 1
     * @param b operand 2
     * @param op operation
     * @param op_32 If to cut off result to 32 bits
     * @return Calculation result
     */
    int64_t alu_exec(int64_t a, int64_t b, ALU_Op_enum op, bool op_32 = false);

    void raiseTrap(CSReg_Cause_t cause, uint64_t tval = 0);

    void setGPR(uint8_t GPR_index, int64_t value) {
        assert(GPR_index >= 0 && GPR_index < 32);
        if (GPR_index) GPR[GPR_index] = value;



        extern uint64_t count;
        extern std::ofstream dump_file;
        if(GPR_index == 18) {
//            dump_file << "[R18] count:" << count << " val:" << value << std::endl;
        }
    }

    bool memRead(uint64_t start_addr, uint64_t size, uint8_t *buffer) {
        if (start_addr % size != 0) {
            CSReg_Cause_t cause;
            cause.cause = exec_load_misalign;
            cause.interrupt = 0;
            raiseTrap(cause, start_addr);
            return false;
        }
        VAddr_RW_Feedback va_err = sv39MMU->VAddr_ReadBuffer_MMU_API(start_addr, size, buffer);
        if (va_err == VADDR_ACCESS_OK) {
            return true;
        }
        else {
            RV64_ExecFeedbackCode_e feedback = (va_err == VADDR_ACCESS_FAULT) ? exec_load_acc_fault :
                                               (va_err == VADDR_MIS_ALIGN) ? exec_load_misalign :
                                               (va_err == VADDR_PAGE_FAULT) ? exec_load_pgfault : exec_ok;
            assert(feedback != exec_ok);
            raiseTrap({ .cause = feedback, .interrupt = 0 }, start_addr);
            return false;
        }
    }

    bool memWrite(uint64_t start_addr, uint64_t size, const uint8_t *buffer) {
        if (start_addr % size != 0) {
//            CSReg_Cause_t cause;
//            cause.cause = exec_store_misalign;
//            cause.interrupt = 0;
            raiseTrap({ .cause = exec_store_misalign, .interrupt = 0 },start_addr);
            return false;
        }
        VAddr_RW_Feedback va_err = sv39MMU->VAddr_WriteBuffer_MMU_API(start_addr,size,buffer);
        if (va_err == VADDR_ACCESS_OK) {
            return true;
        }
        else {
//            CSReg_Cause_t cause;
//            cause.cause = va_err;
//            cause.interrupt = 0;
//            assert(0);
            RV64_ExecFeedbackCode_e feedback = (va_err == VADDR_ACCESS_FAULT) ? exec_store_acc_fault :
                                               (va_err == VADDR_MIS_ALIGN) ? exec_store_misalign :
                                               (va_err == VADDR_PAGE_FAULT) ? exec_store_pgfault : exec_ok;
            assert(feedback != exec_ok);
            raiseTrap({ .cause = feedback, .interrupt = 0 }, start_addr);
            return false;
        }
    }

    void ecall();
    void ebreak();
    bool mret();
    bool sret();
    bool sfence_vma(uint64_t vaddr, uint64_t asid);

    void reset();
    bool csr_read(RV_CSR_Addr_enum csr_index, uint64_t &csr_result);
    bool csr_write(RV_CSR_Addr_enum csr_index, uint64_t csr_data);
    bool csr_setbit(RV_CSR_Addr_enum csr_index, uint64_t csr_mask);
    bool csr_clearbit(RV_CSR_Addr_enum csr_index, uint64_t csr_mask);
    bool csr_op_permission_check(uint16_t csr_index, bool write);
};



