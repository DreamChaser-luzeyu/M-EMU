#pragma once

#include <cassert>
#include <cstring>
#include "interface/MMIO_Dev.h"



class MEmu_MMIO_Mem : public MMIODev_I {
    // ----- Fields
private:
    AddrType_enum acceptAddressType;
    uint8_t* memData;
//    size_t memorySize;

    // ----- Interface implementation
public:

    FuncReturnFeedback_t ReadBuffer_MMIODev_API(uint64_t begin_addr, size_t size, uint8_t *buffer) override {
        assert(begin_addr >= 0);
        if(!buffer) { return MEMU_INVALID_PARAM; }
        uint64_t begin_index = this->acceptAddressType == MMIO_DEV_ADDRESS_TYPE_RELATIVE ?
                               begin_addr : (begin_addr - (this->devBaseAddr));
        // TODO: Handle invalid address accessing

        memcpy(buffer, &((this->memData)[begin_index]), size);
        return MEMU_OK;
    }

    FuncReturnFeedback_t WriteBuffer_MMIODev_API(uint64_t begin_addr, size_t size, uint8_t *buffer) override {
        assert(begin_addr >= 0);
        if(!buffer) { return MEMU_INVALID_PARAM; }
        uint64_t begin_index = this->acceptAddressType == MMIO_DEV_ADDRESS_TYPE_RELATIVE ?
                               begin_addr : (begin_addr - (this->devBaseAddr));
        memcpy(&((this->memData)[begin_index]), buffer, size);
        return MEMU_OK;
    }

    AddrType_enum getAddressingMode() override {
        return this->acceptAddressType;
    }

    // ----- Constructor & Destructor
public:
    MEmu_MMIO_Mem(size_t memory_size = 1024 * 1024 * 128,
                  AddrType_enum addressing_mode = MMIO_DEV_ADDRESS_TYPE_RELATIVE):
        acceptAddressType(addressing_mode) {
        memRegionSize = memory_size;    // Initialize field in super class
        memData = new uint8_t[memory_size];
    }

    ~MEmu_MMIO_Mem() {
        delete memData;
    }
};