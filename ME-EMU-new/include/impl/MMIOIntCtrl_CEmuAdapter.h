#pragma once

#include "interface/IntCtrl.h"

class CemuIntCtrlAdapter : public IntCtrl_I {
    // ----- Fields
private:
    IntCtrl_I* intCtrl;
    std::map<uint32_t, MMIODev_I*> intDevMap;


    // ----- Interface implementation
public:
    FuncReturnFeedback_t ReadBuffer_MMIODev_API (uint64_t begin_addr, uint64_t size, uint8_t* buffer) override {
        // Check if relative address in range
        assert(begin_addr + size <= this->addrRegionSize);   // TODO: Replace with panic function
        // Do access device
        return intCtrl->ReadBuffer_MMIODev_API(begin_addr, size, buffer);
    }
    // begin_addr should be in range [0, addrRegionSize]
    FuncReturnFeedback_t WriteBuffer_MMIODev_API(uint64_t begin_addr, uint64_t size, const uint8_t* buffer) override {
        // Check if relative address in range
        assert(begin_addr + size <= this->addrRegionSize);   // TODO: Replace with panic function
        // Do access device
        return intCtrl->WriteBuffer_MMIODev_API(begin_addr, size, buffer);
    }

    FuncReturnFeedback_t SetCoreState_IntCtrl_API() override {
        return MEMU_OK;
    }

    FuncReturnFeedback_t RegisterDev_IntCtrl_API(uint32_t int_id, MMIODev_I *dev) override {
        // Interrupt already allocated!
        if(intDevMap.find(int_id) != intDevMap.end()) { return MEMU_INTNUM_CONFLICT; }
        intDevMap[int_id] = dev;
        return MEMU_OK;
    }

    FuncReturnFeedback_t UnregisterDev_IntCtrl_API(uint32_t int_id) override {
        auto it = intDevMap.find(int_id);
        // Check if interrupt `int_id` allocated
        if(it == intDevMap.end()) { return MEMU_NOT_FOUND; }
        intDevMap.erase(int_id);
        return MEMU_OK;
    }

    FuncReturnFeedback_t UnregisterDev_IntCtrl_API(MMIODev_I *dev) override {
        bool found = false;
        uint32_t int_id;
        for(auto p : intDevMap) {
            if(p.second == dev) {
                int_id = p.first;
                found = true;
                break;
            }
        }
        if(found) { return UnregisterDev_IntCtrl_API(int_id); }
        return MEMU_NOT_FOUND;
    }

    FuncReturnFeedback_t UpdateIntState_IntCtrl_API() override {
        return MEMU_OK;
    }
    // ----- Constructor & Destructor
public:
    explicit CemuIntCtrlAdapter(mmio_dev* cemu_dev, uint64_t size):
            IntCtrl_I(0x00, size), intCtrl(cemu_dev) {}

    ~CemuIntCtrlAdapter() { /*delete this->intCtrl;*/ }

private:
    CemuIntCtrlAdapter() : IntCtrl_I(0x00, 0x00) {}

    // ----- Member functions
public:
    mmio_dev* getCEMUDev() { return this->intCtrl; }
};