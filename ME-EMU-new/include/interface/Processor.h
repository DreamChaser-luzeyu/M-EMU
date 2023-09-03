#pragma once

#include <ostream>
#include <vector>

#include "misc/status_enum.h"

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

class ProcessorCore_I {
public:
    /**
     * @brief Dump core registers to an array
     * @param regs The array to store the dump of core registers
     */
    virtual FuncReturnFeedback_t DumpRegister_CoreAPI(std::vector<RegisterItem_t>& regs) = 0;
    /**
     * @brief Do fetch-and-execute an instruct
     */
    virtual FuncReturnFeedback_t Step_CoreAPI() = 0;

    virtual FuncReturnFeedback_t DumpProgramCounter_CoreAPI(RegisterItem_t& reg) = 0;

    virtual FuncReturnFeedback_t WriteProgramCounter_CoreAPI(const RegisterItem_t& reg) = 0;
};