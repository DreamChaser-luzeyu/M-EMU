#pragma once

#include <map>
#include <assert.h>

#include "interface/IntCtrl.h"
#include "RVCoreLocalInt.hpp"
#include "rv64_int_status.h"


//struct IntStatus {
//    bool meip;
//    bool msip;
//    bool mtip;
//    bool seip;
//};
//typedef IntStatus IntStatus_t;

class CemuClintAdapter : public IntCtrl_I {
    // ----- Fields
private:
    RVCoreLocalInt<2>* intCtrl;
    std::map<uint32_t, MMIODev_I*> intDevMap;

    // designed for dual-core
    IntStatus_t* core0IntStatus;
    IntStatus_t* core1IntStatus;

    // ----- Interface implementation & override
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
        intCtrl->tick();
        this->core0IntStatus->msip = intCtrl->m_s_irq(0);
        this->core0IntStatus->mtip = intCtrl->m_t_irq(0);
        this->core1IntStatus->msip = intCtrl->m_s_irq(1);
        this->core1IntStatus->mtip = intCtrl->m_t_irq(1);
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
    explicit CemuClintAdapter(RVCoreLocalInt<2>* cemu_dev, uint64_t size):
            IntCtrl_I(0x00, size), intCtrl(cemu_dev) {}

    ~CemuClintAdapter() { /*delete this->intCtrl;*/ }

private:
    CemuClintAdapter() : IntCtrl_I(0x00, 0x00) { assert(0); }

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
