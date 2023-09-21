#pragma once

typedef union {
    struct {
        uint64_t blank0: 1;
        uint64_t s_s_ip: 1; // 1
        uint64_t blank1: 1;
        uint64_t m_s_ip: 1; // 3
        uint64_t blank2: 1;
        uint64_t s_t_ip: 1; // 5
        uint64_t blank3: 1;
        uint64_t m_t_ip: 1; // 7
        uint64_t blank4: 1;
        uint64_t s_e_ip: 1; // 9
        uint64_t blank5: 1;
        uint64_t m_e_ip: 1; // 11
    };
    uint64_t val;
} CSReg_IntPending_t;

typedef union {
    struct {
        uint64_t blank0 :1;
        uint64_t sie    :1;     // supervisor interrupt enable
        uint64_t blank1 :1;
        uint64_t mie    :1;     // machine interrupt enable
        uint64_t blank2 :1;
        uint64_t spie   :1;     // sie prior to trapping
        uint64_t ube    :1;     // u big-endian, zero
        uint64_t mpie   :1;     // mie prior to trapping
        uint64_t spp    :1;     // supervisor previous privilege mode.
        uint64_t vs     :2;     // without vector, zero
        uint64_t mpp    :2;     // machine previous privilege mode.
        uint64_t fs     :2;     // without float, zero
        uint64_t xs     :2;     // without user ext, zero
        uint64_t mprv   :1;     // Modify PRiVilege (Turn on virtual memory and protection for load/store in M-Mode) when mpp is not M-Mode
                                // mprv will be used by OpenSBI.
        uint64_t sum    :1;     // permit Supervisor User Memory access
        uint64_t mxr    :1;     // Make eXecutable Readable
        uint64_t tvm    :1;     // Trap Virtual Memory (raise trap when sfence.vma and sinval.vma executing in S-Mode)
        uint64_t tw     :1;     // Timeout Wait for WFI
        uint64_t tsr    :1;     // Trap SRET
        uint64_t blank3 :9;
        uint64_t uxl    :2;     // user xlen
        uint64_t sxl    :2;     // supervisor xlen
        uint64_t sbe    :1;     // s big-endian
        uint64_t mbe    :1;     // m big-endian
        uint64_t blank4 :25;
        uint64_t sd     :1;     // no vs,fs,xs, zero
    };
    uint64_t val;
} CSReg_MStatus_t;


typedef union {
    struct {
        uint64_t blank0 :1;
        uint64_t sie    :1;     // supervisor interrupt enable
        uint64_t blank1 :3;
        uint64_t spie   :1;     // sie prior to trapping
        uint64_t ube    :1;     // u big-endian, zero
        uint64_t blank2 :1;     // mie prior to trapping
        uint64_t spp    :1;     // supervisor previous privilege mode.
        uint64_t vs     :2;     // without vector, zero
        uint64_t blank3 :2;     // machine previous privilege mode.
        uint64_t fs     :2;     // without float, zero
        uint64_t xs     :2;     // without user ext, zero
        uint64_t blank4 :1;
        uint64_t sum    :1;     // permit Supervisor User Memory access
        uint64_t mxr    :1;     // Make eXecutable Readable
        uint64_t blank5 :12;
        uint64_t uxl    :2;     // user xlen
        uint64_t blank6 :29;
        uint64_t sd     :1;     // no vs,fs,xs, zero
    };
    uint64_t val;
} CSReg_SStatus_t;

typedef union {
    struct {
        uint64_t mode   : 2;    // 0 - Direct    1 - Vectored   2 - Reserved
        uint64_t base   : 62;
    };
    uint64_t val;
} CSReg_TrapVector_t;

typedef union {
    struct {
        uint64_t cause: 63;
        uint64_t interrupt: 1;    // If the trap is caused by an interrupt
    };
    uint64_t val;
} CSReg_Cause_t;

typedef enum {
    U_MODE = 0,
    S_MODE = 1,
    H_MODE = 2,
    M_MODE = 3
} PrivMode_t;

typedef union {
    struct rv_r {
        unsigned int opcode    :7;
        unsigned int rd        :5;
        unsigned int funct3    :3;
        unsigned int rs1       :5;
        unsigned int rs2       :5;
        unsigned int funct7    :7;
    } r_type;
    struct rv_i {
        unsigned int opcode    :7;
        unsigned int rd        :5;
        unsigned int funct3    :3;
        unsigned int rs1       :5;
        int imm12              :12;
    } i_type;
    struct rv_s {
        unsigned int opcode    :7;
        unsigned int imm_4_0   :5;
        unsigned int funct3    :3;
        unsigned int rs1       :5;
        unsigned int rs2       :5;
        int imm_11_5           :7;
    } s_type;
    struct rv_b {
        unsigned int opcode    :7;
        unsigned int imm_11    :1;
        unsigned int imm_4_1   :4;
        unsigned int funct3    :3;
        unsigned int rs1       :5;
        unsigned int rs2       :5;
        unsigned int imm_10_5  :6;
        int imm_12             :1;
    } b_type;
    struct rv_u {
        unsigned int opcode    :7;
        unsigned int rd        :5;
        int imm_31_12          :20;
    } u_type;
    struct rv_j {
        unsigned int opcode    :7;
        unsigned int rd        :5;
        unsigned int imm_19_12 :8;
        unsigned int imm_11    :1;
        unsigned int imm_10_1  :10;
        int imm_20             :1;
    } j_type;
    uint32_t val;
} RVInst_t;

typedef enum {
    exc_instr_misalign          = 0,
    exc_instr_acc_fault         = 1,
    exc_illegal_instr           = 2,
    exc_breakpoint              = 3,
    exc_load_misalign           = 4,
    exc_load_acc_fault          = 5,
    exc_store_misalign          = 6,    // including amo
    exc_store_acc_fault         = 7,    // including amo
    exc_ecall_from_user         = 8,
    exc_ecall_from_supervisor   = 9,
    exc_ecall_from_machine      = 11,
    exc_instr_pgfault           = 12,
    exc_load_pgfault            = 13,
    exc_store_pgfault           = 15,   // including amo
    EXEC_OK               = 24
} ExecFeedbackCode_e;

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
typedef struct {
    uint64_t ppn    : 44;
    uint64_t asid   : 16;
    uint64_t mode   : 4;
} SATP_Reg_t;

typedef struct {
    uint64_t page_off   : 12;
    uint64_t vpn_0      : 9;
    uint64_t vpn_1      : 9;
    uint64_t vpn_2      : 9;
    uint64_t blank      : 25;
} SV39_VAddr_t;
