#pragma once

#include "interface/MMIO_Bus.h"

#include "misc/status_enum.h"

enum VAddr_RW_Feedback : uint16_t ;
typedef enum VAddr_RW_Feedback VAddr_RW_Feedback_e;

class MMU_I {
    // ----- Fields
private:
    MMIOBus_I* sysBus;
    // ----- Interface
public:
    virtual VAddr_RW_Feedback_e PAddr_ReadBuffer_MMU_API(uint64_t begin_addr, uint64_t size, uint8_t* buffer) = 0;
    virtual VAddr_RW_Feedback_e PAddr_WriteBuffer_MMU_API(uint64_t begin_addr, uint64_t size, uint8_t* buffer) = 0;
    // ----- Constructor & Destructor
private:
    MMU_I() {}
public:
    MMU_I(MMIOBus_I* sys_bus) : sysBus(sys_bus) {}
};