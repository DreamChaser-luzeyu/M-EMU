#pragma once
#include <cstdint>
#include <cstddef>
#include "misc/status_enum.h"

typedef enum {
    MMIO_DEV_ADDRESS_TYPE_RELATIVE = 0,
    MMIO_DEV_ADDRESSING_MODE_RAW
} AddrType_enum;


class MMIODev_I {
    // ----- Fields
protected:
    uint64_t devBaseAddr;
    size_t memRegionSize;
    // ----- Interfaces
public:
    virtual FuncReturnFeedback_t ReadBuffer_MMIODev_API (uint64_t begin_addr, size_t size, uint8_t* buffer) = 0;

    virtual FuncReturnFeedback_t WriteBuffer_MMIODev_API(uint64_t begin_addr, size_t size, uint8_t* buffer) = 0;

    virtual AddrType_enum getAddressingMode() = 0;

    // ----- Member functions
public:
    void setDevBaseAddr(uint64_t dev_base_addr) { this->devBaseAddr = dev_base_addr; }

    size_t getDevMemRegionSize() const { return this->memRegionSize; }

    uint64_t getDevBaseAddr() const { return this->devBaseAddr; }

    bool isAddrRegionConflict(MMIODev_I* dev) const {
        uint64_t dev1_begin_addr = this->devBaseAddr;
        uint64_t dev1_end_addr   = dev1_begin_addr + this->memRegionSize;
        uint64_t dev2_begin_addr = dev->devBaseAddr;
        uint64_t dev2_end_addr   = dev2_begin_addr + dev->memRegionSize;
        if(dev1_begin_addr <= dev2_begin_addr && dev2_begin_addr <= dev1_end_addr) { return true; }
        if(dev1_begin_addr <= dev2_end_addr && dev2_end_addr <= dev1_end_addr) { return true; }
        return false;
    }

    bool isAddrInRange(uint64_t addr) { return (devBaseAddr <= addr) && (addr <= (devBaseAddr + memRegionSize)); }
};

typedef MMIODev_I* MMIODevHandle_t;