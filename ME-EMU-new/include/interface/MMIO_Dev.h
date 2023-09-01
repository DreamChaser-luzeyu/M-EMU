#pragma once
#include <cstdint>
#include <cstddef>
#include "misc/status_enum.h"

//typedef enum {
//    MMIO_DEV_ADDRESS_TYPE_RELATIVE = 0,
//    MMIO_DEV_ADDRESS_TYPE_CIRCULAR
//} AddrType_enum;


class MMIODev_I {
    // ----- Fields
protected:
    // TODO: Move to BUS
    uint64_t devBaseAddr;
    uint64_t addrRegionSize;

//    AddrType_enum addressingMode;
    // ----- Interfaces
public:
    // The `begin_addr` and `begin_addr + size` should be in region [0, addrRegionSize]
    virtual FuncReturnFeedback_t ReadBuffer_MMIODev_API (uint64_t begin_addr, uint64_t size, uint8_t* buffer) = 0;

    virtual FuncReturnFeedback_t WriteBuffer_MMIODev_API(uint64_t begin_addr, uint64_t size, const uint8_t* buffer) = 0;



    // ----- Constructor & Destructor
private:
    MMIODev_I() {} ///< (Explicitly disabled)

protected:
    MMIODev_I(uint64_t devBaseAddr, uint64_t memRegionSize) : devBaseAddr(devBaseAddr), addrRegionSize(memRegionSize) {}

    // ----- Member functions
public:
    void setDevBaseAddr(uint64_t dev_base_addr) { this->devBaseAddr = dev_base_addr; }

    uint64_t getDevMemRegionSize() const { return this->addrRegionSize; }

    uint64_t getDevBaseAddr() const { return this->devBaseAddr; }

    bool isAddrRegionConflict(MMIODev_I* dev) const {
        uint64_t dev1_begin_addr = this->devBaseAddr;
        uint64_t dev1_end_addr   = dev1_begin_addr + this->addrRegionSize;
        uint64_t dev2_begin_addr = dev->devBaseAddr;
        uint64_t dev2_end_addr   = dev2_begin_addr + dev->addrRegionSize;
        if(dev1_begin_addr <= dev2_begin_addr && dev2_begin_addr <= dev1_end_addr) { return true; }
        if(dev1_begin_addr <= dev2_end_addr && dev2_end_addr <= dev1_end_addr) { return true; }
        return false;
    }

    bool isAddrInRange(uint64_t addr) { return (devBaseAddr <= addr) && (addr <= (devBaseAddr + addrRegionSize)); }

//    AddrType_enum getAddressingMode() { return this->addressingMode; }
};

typedef MMIODev_I* MMIODevHandle_t;