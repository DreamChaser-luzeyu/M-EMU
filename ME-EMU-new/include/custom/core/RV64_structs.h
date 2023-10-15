#pragma once

typedef enum {
    U_MODE = 0,
    S_MODE = 1,
    H_MODE = 2,
    M_MODE = 3
} PrivMode_e;



typedef enum {
    exec_instr_misalign          = 0,
    exec_instr_acc_fault         = 1,
    exec_illegal_instr           = 2,
    exec_breakpoint              = 3,
    exec_load_misalign           = 4,
    exec_load_acc_fault          = 5,
    exec_store_misalign          = 6,    // including amo
    exec_store_acc_fault         = 7,    // including amo
    exec_ecall_from_user         = 8,
    exec_ecall_from_supervisor   = 9,
    exec_ecall_from_machine      = 11,
    exec_instr_pgfault           = 12,
    exec_load_pgfault            = 13,
    exec_store_pgfault           = 15,   // including amo
    exec_ok               = 24
} RV64_ExecFeedbackCode_e;

typedef enum  {
    int_s_sw    = 1,
    int_m_sw    = 3,
    int_s_timer = 5,
    int_m_timer = 7,
    int_s_ext   = 9,
    int_m_ext   = 11,
    int_no_int  = 24
} IntType_e;


// Supervisor Address Translation and Protection (satp) Register
typedef union {
    struct {
        uint64_t ppn: 44;
        uint64_t asid: 16;
        uint64_t mode: 4;
    };
    uint64_t val;
} SATP_Reg_t;







