#pragma once

#include <cassert>
#include <cstring>
#include "interface/MMIO_Dev.h"

typedef uint64_t size_t;

class MEmu_MMIO_Mem : public MMIODev_I {
    // ----- Fields
private:
    uint8_t* memData;
    uint64_t memorySize;

    // ----- Interface implementation
public:

    FuncReturnFeedback_e ReadBuffer_MMIODev_API(uint64_t begin_addr, uint64_t size, uint8_t *buffer) override {
        assert(begin_addr >= 0);
        if(!buffer) { return MEMU_INVALID_PARAM; }
        uint64_t begin_index = begin_addr;
        // TODO: Handle invalid address accessing
        memcpy(buffer, &((this->memData)[begin_index]), size);
        return MEMU_OK;
    }

    FuncReturnFeedback_e WriteBuffer_MMIODev_API(uint64_t begin_addr, uint64_t size, const uint8_t *buffer) override {
        // TODO: check if end addr in range
        assert(begin_addr >= 0);
        if(!buffer) { return MEMU_INVALID_PARAM; }
        uint64_t begin_index = begin_addr;
        memcpy(&((this->memData)[begin_index]), buffer, size);
        return MEMU_OK;
    }

    // ----- Constructor & Destructor
public:
    explicit MEmu_MMIO_Mem(uint64_t memory_size = 1024 * 1024 * 128): MMIODev_I(0x00, memory_size) {
        addrRegionSize = memory_size;
        // For memory, we just map address liner, so the `addrRegionSize` should be exactly size of memory or smaller.
        memorySize = memory_size;        // TODO: We should ensure addrRegionSize less equal to memorySize
        memData = new uint8_t[memory_size];
    }

    ~MEmu_MMIO_Mem() {
        delete memData;
    }
};