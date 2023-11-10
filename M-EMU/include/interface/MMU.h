#pragma once

#include "sdk/symbol_attr.h"

#include "interface/MMIO_Bus.h"

#include "misc/status_enum.h"

enum VAddr_RW_Feedback : uint16_t ;
typedef enum VAddr_RW_Feedback VAddr_RW_Feedback_e;

class MMU_I {
    // ----- Fields
protected:
    MMIOBus_I* sysBus;
    // ----- Interface
public:
    virtual ALWAYS_INLINE inline VAddr_RW_Feedback_e VAddr_ReadBuffer_MMU_API(uint64_t begin_addr, uint64_t size, uint8_t *buffer) = 0;
    virtual ALWAYS_INLINE inline VAddr_RW_Feedback_e VAddr_WriteBuffer_MMU_API(uint64_t begin_addr, uint64_t size, const uint8_t* buffer) = 0;
    virtual ALWAYS_INLINE inline VAddr_RW_Feedback_e VAddr_InstFetch_MMU_API(uint64_t begin_addr, uint64_t size, void* buffer) = 0;
    // ----- Constructor & Destructor
private:
    MMU_I() {}
protected:
    MMU_I(MMIOBus_I* sys_bus) : sysBus(sys_bus) {}
};