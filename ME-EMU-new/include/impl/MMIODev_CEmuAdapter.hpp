#pragma once

#include "interface/MMIO_Dev.h"

#include "custom/misc/uartlite.hpp"
#include "custom/misc/mmio_dev.hpp"


class CemuDevAdapter : public MMIODev_I {
    // ----- Fields
private:
    mmio_dev* cemuDev;
    // ----- Interface implementation
public:
    FuncReturnFeedback_t ReadBuffer_MMIODev_API (uint64_t begin_addr, uint64_t size, uint8_t* buffer) override {
        // Check if relative address in range
        assert(begin_addr + size <= this->addrRegionSize);   // TODO: Replace with panic function
        // Do access device
        bool feedback = cemuDev->do_read(begin_addr, size, buffer);
        if(!feedback) { return MEMU_UNKNOWN; }
        return MEMU_OK;
    }
    // begin_addr should be in range [0, addrRegionSize]
    FuncReturnFeedback_t WriteBuffer_MMIODev_API(uint64_t begin_addr, uint64_t size, const uint8_t* buffer) override {
        // Check if relative address in range
        assert(begin_addr + size <= this->addrRegionSize);   // TODO: Replace with panic function
        // Do access device
        bool feedback = cemuDev->do_write(begin_addr, size, buffer);
        if(!feedback) { return MEMU_UNKNOWN; }
        return MEMU_OK;
    }

    bool GetIrqStatus() override { return cemuDev->irq(); }
    // ----- Constructor & Destructor
public:
    explicit CemuDevAdapter(mmio_dev* cemu_dev, uint64_t size): MMIODev_I(0x00, size), cemuDev(cemu_dev) {}

    ~CemuDevAdapter() { delete this->cemuDev; }
private:
    CemuDevAdapter() : MMIODev_I(0x00, 0x00) {}

    // ----- Member functions
public:
    mmio_dev* getCEMUDev() { return this->cemuDev; }
};
