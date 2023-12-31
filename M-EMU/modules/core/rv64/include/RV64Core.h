#pragma once

#include "interface/Core.h"
#include "interface/MMIO_Bus.h"
#include "rv64_structs.h"
#include "rv64_csr_structs.h"
#include "interface/MMU.h"
#include "RV64SV39_MMU.h"
#include "rv64_int_status.h"

#include <iostream>
#include <fstream>

enum ALU_Op_enum {
    ALU_ADD, ALU_SUB, ALU_SLL, ALU_SLT, ALU_SLTU, ALU_XOR, ALU_SRL, ALU_SRA, ALU_OR, ALU_AND,
    ALU_MUL, ALU_MULH, ALU_MULHU, ALU_MULHSU, ALU_DIV, ALU_DIVU, ALU_REM, ALU_REMU,
    ALU_NOP
};

class RV64Core : public ProcessorCore_I {
    // ----- Constants
    const uint64_t S_MODE_INT_MASK = (1ull << int_s_ext) | (1ull << int_s_sw) | (1ull << int_s_timer);
    const uint64_t COUNTER_MASK = (1 << 0) | (1 << 2);
    const uint64_t S_MODE_EXC_MASK = (1 << 16) - 1 - (1 << exec_ecall_from_machine);
    const uint64_t M_MODE_INT_MASK = S_MODE_INT_MASK | (1ull << int_m_ext) | (1ull << int_m_sw) | (1ull << int_m_timer);
    const uint8_t PC_ALIGN = 2;
    // the name GDB should use when describing these register
    const std::vector<std::string> GPR_NAME_ARR = {
            "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
            "fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
            "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
            "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
    };
    const std::string PC_NAME = "pc";
    // ----- Fields
private:
    RV64SV39_MMU* sv39MMU;

    // ----- Regs
    int64_t GPR[32];                                   // General purpose registers
    // ----- CSRs
    // --- M_MODE CSRs
    uint64_t                csrMCycleNum;

    uint64_t                csrMIntDelegation;         ///< mideleg, Machine-level Interrupt Delegation Register
                                                       ///< Used as a mask, indicating which traps to be routed to S_MODE
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


    // ----- Interface ProcessorCore_I implementations
public:
    FuncReturnFeedback_e Step_CoreAPI() override;
    FuncReturnFeedback_e DumpProgramCounter_CoreAPI(RegisterItem_t& reg) override;
    FuncReturnFeedback_e WriteProgramCounter_CoreAPI(RegItemVal_t reg_val) override;
    FuncReturnFeedback_e SetGPRByIndex_CoreAPI(uint8_t gpr_index, int64_t val) override;
    // ----- Interface Debuggable_I implementations
//    FuncReturnFeedback_e VAddrRead_CoreDebugAPI(uint64_t start_addr, uint64_t size, uint8_t *buffer) override;
//    FuncReturnFeedback_e VAddrWrite_CoreDebugAPI(uint64_t start_addr,
//                                                 uint64_t size, const uint8_t *buffer) override;
    FuncReturnFeedback_e MemRead_CoreDebugAPI(uint64_t start_addr, uint64_t size, uint8_t *buffer) override;
    FuncReturnFeedback_e MemWrite_CoreDebugAPI(uint64_t start_addr,
                                               uint64_t size, const uint8_t *buffer) override;
    FuncReturnFeedback_e DumpRegister_CoreAPI(std::vector<RegisterItem_t>& regs) override;
    FuncReturnFeedback_e WriteAllRegister_CoreAPI(const std::vector<RegisterItem_t>& regs) override;

    // ----- Constructors & Destructors
    RV64Core(MMIOBus_I* sys_bus, uint16_t hart_id) :
        rvHartID(hart_id), csrMCycleNum(0), needTrap(false) {
        this->intStatus = new IntStatus_t;
        intStatus->mtip = false;
        intStatus->msip = false;
        intStatus->meip = false;
        intStatus->seip = false;
        this->sv39MMU = new RV64SV39_MMU(sys_bus, currentPrivMode, satp, status, rvHartID);
        reset();
    }

    // ----- Member functions
private:
    ALWAYS_INLINE inline void preExec();

    /**
     * Do calculations
     * @param a operand 1
     * @param b operand 2
     * @param op operation
     * @param op_32 If to cut off result to 32 bits
     * @return Calculation result
     */
    ALWAYS_INLINE inline int64_t alu_exec(int64_t a, int64_t b, ALU_Op_enum op, bool op_32 = false);
    ALWAYS_INLINE inline void raiseTrap(CSReg_Cause_t cause, uint64_t tval = 0);
    ALWAYS_INLINE inline void setGPR(uint8_t GPR_index, int64_t value) {
        assert(GPR_index >= 0 && GPR_index < 32);
        if (likely(GPR_index)) GPR[GPR_index] = value;
    }
    ALWAYS_INLINE inline bool memRead(uint64_t start_addr, uint64_t size, uint8_t *buffer) {
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
    ALWAYS_INLINE inline bool memWrite(uint64_t start_addr, uint64_t size, const uint8_t *buffer) {
        if (start_addr % size != 0) {
            raiseTrap({ .cause = exec_store_misalign, .interrupt = 0 },start_addr);
            return false;
        }
        VAddr_RW_Feedback va_err = sv39MMU->VAddr_WriteBuffer_MMU_API(start_addr,size,buffer);
        if (va_err == VADDR_ACCESS_OK) {
            return true;
        }
        else {
            RV64_ExecFeedbackCode_e feedback = (va_err == VADDR_ACCESS_FAULT) ? exec_store_acc_fault :
                                               (va_err == VADDR_MIS_ALIGN) ? exec_store_misalign :
                                               (va_err == VADDR_PAGE_FAULT) ? exec_store_pgfault : exec_ok;
            assert(feedback != exec_ok);
            raiseTrap({ .cause = feedback, .interrupt = 0 }, start_addr);
            return false;
        }
    }

    ALWAYS_INLINE inline void ecall();
    ALWAYS_INLINE inline void ebreak();
    ALWAYS_INLINE inline bool mret();
    ALWAYS_INLINE inline bool sret();
    ALWAYS_INLINE inline bool sfence_vma(uint64_t vaddr, uint64_t asid);

    ALWAYS_INLINE inline bool csr_read(RV_CSR_Addr_enum csr_index, uint64_t &csr_result);
    ALWAYS_INLINE inline bool csr_write(RV_CSR_Addr_enum csr_index, uint64_t csr_data);
    ALWAYS_INLINE inline bool csr_setbit(RV_CSR_Addr_enum csr_index, uint64_t csr_mask);
    ALWAYS_INLINE inline bool csr_clearbit(RV_CSR_Addr_enum csr_index, uint64_t csr_mask);
    ALWAYS_INLINE inline bool csr_op_permission_check(uint16_t csr_index, bool write);

    void reset();
};

// We have to include those implements here because those functions are marked `inline`
// so they have to be placed in ONE document
#include "rv64_common.hpp"
#include "rv64_privilege.hpp"

