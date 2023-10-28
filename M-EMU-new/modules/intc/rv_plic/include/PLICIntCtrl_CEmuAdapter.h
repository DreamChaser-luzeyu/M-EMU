#pragma once

#include "interface/IntCtrl.h"
#include "rv64_int_status.h"
#include "RVPLICtrl.hpp"
#include <map>

class CemuPlicAdapter : public IntCtrl_I {
    // ----- Fields
private:
    RVPLICtrl<4,4>* intCtrl;
    std::map<uint32_t, MMIODev_I*> intDevMap;

    // designed for dual-core
    IntStatus_t* core0IntStatus;
    IntStatus_t* core1IntStatus;

    // ----- Interface implementation
public:
    FuncReturnFeedback_e ReadBuffer_MMIODev_API (uint64_t begin_addr, uint64_t size, uint8_t* buffer) override {
        // Check if relative address in range
        assert(begin_addr + size <= this->addrRegionSize);   // TODO: Replace with panic function
        // Do access device
        bool feedback = intCtrl->do_read(begin_addr, size, buffer);
        if(feedback) { return MEMU_OK; }
        return MEMU_UNKNOWN;
    }
    // begin_addr should be in range [0, addrRegionSize]
    FuncReturnFeedback_e WriteBuffer_MMIODev_API(uint64_t begin_addr, uint64_t size, const uint8_t* buffer) override {
        // Check if relative address in range
        assert(begin_addr + size <= this->addrRegionSize);   // TODO: Replace with panic function
        // Do access device
        bool feedback = intCtrl->do_write(begin_addr, size, buffer);
        if(feedback) { return MEMU_OK; }
        return MEMU_UNKNOWN;
    }

    FuncReturnFeedback_e SetCoreState_IntCtrl_API() override {
        return MEMU_OK;
    }

    FuncReturnFeedback_e RegisterDev_IntCtrl_API(uint32_t int_id, MMIODev_I *dev) override {
        // Interrupt already allocated!
        if(intDevMap.find(int_id) != intDevMap.end()) { return MEMU_INTNUM_CONFLICT; }
        intDevMap[int_id] = dev;
        return MEMU_OK;
    }

    FuncReturnFeedback_e UnregisterDev_IntCtrl_API(uint32_t int_id) override {
        auto it = intDevMap.find(int_id);
        // Check if interrupt `int_id` allocated
        if(it == intDevMap.end()) { return MEMU_NOT_FOUND; }
        intDevMap.erase(int_id);
        return MEMU_OK;
    }

    FuncReturnFeedback_e UnregisterDev_IntCtrl_API(MMIODev_I *dev) override {
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

    FuncReturnFeedback_e UpdateIntState_IntCtrl_API() override {
        for(auto it : intDevMap) {
            intCtrl->update_ext((int)(it.first), it.second->GetIrqStatus());
        }

        this->core0IntStatus->meip = intCtrl->get_int(0);
        this->core0IntStatus->seip = intCtrl->get_int(1);
        this->core1IntStatus->meip = intCtrl->get_int(2);
        this->core1IntStatus->seip = intCtrl->get_int(3);

        return MEMU_OK;
    }

    FuncReturnFeedback_e AttachCoreIntStatus(IntStatus_t *int_status, uint64_t hart_id) override {
//        intStatus.resize(2);
        FuncReturnFeedback_e feedback = IntCtrl_I::AttachCoreIntStatus(int_status, hart_id);
        if(feedback == MEMU_OK) {
            core0IntStatus = intStatus.at(0);
            core1IntStatus = intStatus.at(1);
        }
        return feedback;
    }

    // ----- Constructor & Destructor
public:
    explicit CemuPlicAdapter(RVPLICtrl<4,4>* cemu_dev, uint64_t size):
        IntCtrl_I(0x00, size), intCtrl(cemu_dev) {}

    ~CemuPlicAdapter() { /*delete this->intCtrl;*/ }

private:
    CemuPlicAdapter() : IntCtrl_I(0x00, 0x00) { assert(0); }

    // ----- Member functions
public:
    mmio_dev* getCEMUDev() { return this->intCtrl; }

//    FuncReturnFeedback_e setCore0IntStatusPtr(IntStatus_t* int_status) {
//        if(!int_status) { assert(0); }
//        this->core0IntStatus = int_status;
//        return MEMU_OK;
//    }
//
//    FuncReturnFeedback_e setCore1IntStatusPtr(IntStatus_t* int_status) {
//        if(!int_status) { assert(0); }
//        this->core1IntStatus = int_status;
//        return MEMU_OK;
//    }

};