#include "custom/core/RV64Core.h"
#include "custom/core/RV64SV39_MMU.h"

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

void RV64Core::preExec() {
    // TODO: fill the raise trap

    // ----- Update CSRs
    this->csrMCycleNum++;
    csrMIntPending.m_e_ip = this->intStatus->meip;
    csrMIntPending.m_s_ip = this->intStatus->msip;
    csrMIntPending.m_t_ip = this->intStatus->mtip;
    csrMIntPending.s_e_ip = this->intStatus->seip;
    // ----- Update inner states
    this->needTrap = false;
    this->currentPrivMode = this->nextPrivMode;
    // ----- Check interrupt bits
    const uint64_t int_bits = (csrMIntPending.val) & csrMIntEnable;
    if(bits_to_int_type(int_bits) == int_no_int) { return; }  // Certainly no need to trap, so return directly
    // ----- Raise trap
    // 1-- For M_MODE
    if(currentPrivMode == M_MODE) {
        uint64_t m_mode_int_bits = int_bits & (~csrMIntDelegation);  // Interrupts that are not delegated to S_MODE
        if(csrMachineStatus.mie && m_mode_int_bits) {
            // M_MODE interrupt enabled && there exists M_MODE interrupt
            // then we want to raise a trap
            // No need to perform mode switching
            CSReg_Cause_t cause;
            cause.cause = bits_to_int_type(m_mode_int_bits);;
            cause.interrupt = 1;
            return raiseTrap(cause);
            // **ends here
        }
    }
    // 2-- For other mode
    else {
        IntType_e int_type = bits_to_int_type(int_bits);
        // Check if exists interrupt to be delegated to S_MODE
        if(int_bits & csrMIntDelegation) {
            // There exists interrupts to be delegated to S_MODE

            if(currentPrivMode < S_MODE) {
                // We are at lower privilege level than S_MODE
                // A trap is needed to go to S_MODE <TO BE VERIFIED>
                CSReg_Cause_t cause;
                cause.cause = int_type;
                cause.interrupt = 1;
                return raiseTrap(cause);
                // **ends here
            }
            else if(csrMachineStatus.sie) {
                // We have S_MODE interrupt enabled, so we want to raise a trap
                CSReg_Cause_t cause;
                cause.cause = int_type;
                cause.interrupt = 1;
                return raiseTrap(cause);
                // ??? What if we are not at S_MODE, but S_MODE interrupt is disabled? Should we raise a trap?
                // I think we need a trap to switch to S_MODE anyway. <TO BE VERIFIED>
                // We need two traps ???
                // **ends here
            }
        }
        else {
            // Does not exist interrupts to be delegated to S_MODE
            // We should trap to M_MODE by default
            if(currentPrivMode < M_MODE) {
                // Same as above, we need a trap to switch to M_MODE
                CSReg_Cause_t cause;
                cause.cause = int_type;
                cause.interrupt = 1;
                return raiseTrap(cause);
                // ** ends here
            }

            else if(csrMachineStatus.mie) {
                // If we have M_MODE interrupt enabled
                // then we use the int_bits directly
                CSReg_Cause_t cause;
                cause.cause = int_type;
                cause.interrupt = 1;
                return raiseTrap(cause);
                // **ends here
            }
        }
    }
    assert(0);  // Should never reach here
}

void RV64Core::raiseTrap(CSReg_Cause_t cause, uint64_t tval)
{
    assert(!this->needTrap);
    needTrap = true;
    uint8_t trap_dest = 0;  // 0 - trap to M_MODE   1 - trap to S_MODE

    // --- Decide which mode to trap to
    if(currentPrivMode != M_MODE) {
        if(cause.interrupt) {
            // Trap caused by interrupt
            if(csrMIntDelegation & (1 << cause.cause)) {
                trap_dest = 1;
            }
        }
        else {
            // Trap caused by exception
            if(csrMExceptionDelegation & (1 << cause.cause)) {
                trap_dest = 1;
            }
        }
    }
    // If in M_MODE, the core do not care the delegated traps
    // so the trap destination should always be M_MODE

    // --- Do trap procedure
    if(trap_dest == 1) {
        // Trap to S_MODE
        csrSupervisorTrapVal = tval;
        csrSupervisorCause.val = cause.val;
        csrSExceptionPC = currentProgramCounter;
        csrSupervisorStatus.spie = csrSupervisorStatus.sie;  // TODO: ???
        csrSupervisorStatus.sie = 0;
        csrSupervisorStatus.spp = (uint64_t)currentPrivMode;
        trapProgramCounter = (csrSTrapVecBaseAddr.base << 2) +
                             ((csrSTrapVecBaseAddr.mode == 1) ? (csrSupervisorCause.cause * 4) : 0);
        nextPrivMode = S_MODE;
    }
    else if(trap_dest == 0) {
        // Trap to M_MODE
        csrMachineTrapVal = tval;
        csrMachineCause.val = cause.val;
        csrMExceptionPC = currentProgramCounter;
        csrMachineStatus.mpie = csrMachineStatus.mie;
        csrMachineStatus.mie = 0;
        csrMachineStatus.mpp = currentPrivMode;
        trapProgramCounter = (csrMTrapVecBaseAddr.base << 2) +
                             ((csrMTrapVecBaseAddr.mode == 1) ? (csrMachineCause.cause * 4) : 0);
        nextPrivMode = M_MODE;
    }
    else {
        assert(0);  // Never reach here
    }

    if(cause.cause == exc_instr_pgfault && tval == trapProgramCounter) { assert(false); }
}
