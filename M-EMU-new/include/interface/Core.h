#pragma once

#include <ostream>
#include <vector>

#include "sdk/symbol_attr.h"
#include "misc/status_enum.h"

typedef union {
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
} RegItemVal_t;

typedef struct {
    uint16_t reg_id;                ///< ID or index of the register
    char disp_name[8];              ///< name of the register
    uint8_t size;                   ///< size (in byte) of the register
    RegItemVal_t val;                          ///< Value of the register
} RegisterItem_t;



struct IntStatus;
typedef IntStatus IntStatus_t;

class ProcessorCore_I {
    // ----- Fields
protected:
    IntStatus_t* intStatus;
public:
    /**
     * @brief Dump core registers to an array
     * @param regs The array to store the dump of core registers
     */
    virtual FuncReturnFeedback_e DumpRegister_CoreAPI(std::vector<RegisterItem_t>& regs) = 0;

    /**
     * @brief Do fetch-and-execute an instruct
     */
    virtual ALWAYS_INLINE inline FuncReturnFeedback_e Step_CoreAPI() = 0;

    virtual FuncReturnFeedback_e DumpProgramCounter_CoreAPI(RegisterItem_t& reg) = 0;

    virtual ALWAYS_INLINE inline FuncReturnFeedback_e WriteProgramCounter_CoreAPI(RegItemVal_t reg_val) = 0;

    virtual ALWAYS_INLINE inline FuncReturnFeedback_e setGPRByIndex_CoreAPI(uint8_t gpr_index, int64_t val) = 0;

    // ----- Member functions
public:
    IntStatus_t* getIntStatusPtr_Core() { return this->intStatus; }
};