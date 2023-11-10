#pragma once

#include <ostream>
#include <vector>
#include <unordered_set>

#include "sdk/symbol_attr.h"
#include "misc/status_enum.h"

typedef union RegItemVal {
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
    RegItemVal_t val;               ///< Value of the register
} RegisterItem_t;

struct IntStatus;
typedef IntStatus IntStatus_t;

// TODO: Split into ProcessorCore_Base and Debuggable_I
class ProcessorCore_I {
    // ----- Fields
protected:
    IntStatus_t* intStatus;
    std::unordered_set<uint64_t> watchPointPCs;
public:
    virtual int GetISABitLen() { return 64; }

    /**
     * @brief Dump core registers to an array
     * @param regs The array to store the dump of core registers
     */
    virtual FuncReturnFeedback_e DumpRegister_CoreAPI(std::vector<RegisterItem_t>& regs) = 0;

    /**
     * @brief Do fetch-and-execute an instruct
     */
    virtual FuncReturnFeedback_e Step_CoreAPI() = 0;

    virtual FuncReturnFeedback_e WriteProgramCounter_CoreAPI(RegItemVal_t reg_val) = 0;

    virtual FuncReturnFeedback_e SetGPRByIndex_CoreAPI(uint8_t gpr_index, int64_t val) = 0;

    virtual FuncReturnFeedback_e DumpProgramCounter_CoreAPI(RegisterItem_t& reg) = 0;

//    virtual FuncReturnFeedback_e VAddrRead_CoreDebugAPI(uint64_t start_addr, uint64_t size, uint8_t *buffer) = 0;
//
//    virtual FuncReturnFeedback_e VAddrWrite_CoreDebugAPI(uint64_t start_addr,
//                                                         uint64_t size, const uint8_t *buffer) = 0;

    virtual FuncReturnFeedback_e MemRead_CoreDebugAPI(uint64_t start_addr, uint64_t size, uint8_t *buffer) = 0;

    virtual FuncReturnFeedback_e MemWrite_CoreDebugAPI(uint64_t start_addr,
                                                       uint64_t size, const uint8_t *buffer) = 0;

    virtual FuncReturnFeedback_e WriteAllRegister_CoreAPI(const std::vector<RegisterItem_t>& regs) = 0;



    // ----- Member functions
public:
    IntStatus_t* GetIntStatusPtr_Core() { return this->intStatus; }

    void InsertWatchPoint_CoreDebug(const RegItemVal_t& reg_val) { watchPointPCs.insert(reg_val.u64_val); }

    void RemoveWatchPoint_CoreDebug(const RegItemVal_t& reg_val) { watchPointPCs.erase(reg_val.u64_val); }

    bool IsCurrentPCAtWatchPoint_CoreDebug() {
        RegisterItem_t pc;
        DumpProgramCounter_CoreAPI(pc);
        auto it = watchPointPCs.find(pc.val.u64_val);
        if(it == watchPointPCs.end()) { return false; }
        return true;
    }

};