#include "custom/core/RV64Core.h"

static inline IntType_e bits_to_int_type(uint64_t int_bits) {
    /*
        According to spec, multiple simultaneous
        interrupts destined for M-mode are handled
        in the following decreasing :
        MEI, MSI, MTI, SEI, SSI, STI.
    */
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

IntType_e RV64Core::preExec() {
    // --- Update CSRs
    this->csrMCycleNum++;
    csrMIntPending.m_e_ip = this->intStatus->meip;
    csrMIntPending.m_s_ip = this->intStatus->msip;
    csrMIntPending.m_t_ip = this->intStatus->mtip;
    csrMIntPending.s_e_ip = this->intStatus->seip;
    // --- Update inner states
    needTrap = false;
    currentPrivMode = nextPrivMode;
    // --- Check and raise interrupt
    uint64_t int_bits = (csrMIntPending.val) & csrMIntEnable;
    IntType_e exec_feedback = int_no_int;
    if(currentPrivMode == M_MODE) {
        uint64_t m_mode_int_bits = int_bits & (~csrMIntDelegation);
        if(csrMachineStatus.mie && m_mode_int_bits) {
            IntType_e int_type = bits_to_int_type(m_mode_int_bits);
        }
    }
    else {
        IntType_e int_type = bits_to_int_type(int_bits);
        // Check if exists int delegation ??
        if(int_bits & csrMIntDelegation) {
            // Has int delegation
            if(csrMachineStatus.sie || currentPrivMode < S_MODE) {
                // Need to raise trap to switch to supervisor mode
                exec_feedback = int_type;
            }
        }
        // No int delegation
        else {
            if(csrMachineStatus.mie || currentPrivMode < M_MODE) {
                // Need to raise trap to switch to machine mode
                exec_feedback = int_type;
            }
        }
    }
    return exec_feedback;
}