// For user implementation
#ifndef ME_EMU_EXEC_H
#define ME_EMU_EXEC_H

typedef struct {
    uint16_t reg_id;                ///< ID or index of the register
    char disp_name[8];              ///< name of the register
    uint8_t size;                   ///< size (in byte) of the register
    union {
        struct {
            uint8_t byte_0;         // Low significant byte
            uint8_t byte_1;
            uint8_t byte_2;
            uint8_t byte_3;
            uint8_t byte_4;
            uint8_t byte_5;
            uint8_t byte_6;
            uint8_t byte_7;         // High significant byte
        };
        struct {
            uint32_t u32_val_0;
            uint32_t u32_val_1;
        };
        uint64_t u64_val;
    } val;                          ///< Value of the register
} RegisterItem_t;

extern uint16_t ProgramCounter_Reg_ID_GV;

/**
 * @brief Reset the core to the initial state
 */
void Reset_CoreAPI();

/**
 * @brief Init the core
 */
void Init_CoreAPI();

/**
 * @brief Do fetch-and-execute an instruct
 */
void Step_CoreAPI();

/**
 * @brief Dump core registers to an array
 * @param regs The array to store the dump of core registers
 * @param nr_regs Number of registers dumped
 */
void DumpRegister_CoreAPI(RegisterItem_t* regs, size_t nr_regs);

#endif //ME_EMU_EXEC_H
