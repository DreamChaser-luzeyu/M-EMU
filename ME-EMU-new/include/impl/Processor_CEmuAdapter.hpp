#pragma once

#include "interface/Processor.h"

struct IntStatus {
    bool meip;
    bool msip;
    bool mtip;
    bool seip;
};
typedef IntStatus IntStatus_t;

class CEmuCoreAdapter : public ProcessorCore_I {
    // ----- Fields
private:
    RVCore* cemuCore;

    bool meip;
    bool msip;
    bool mtip;
    bool seip;

    rv_systembus busAdapter;
    // ----- Interface implementation
public:
    FuncReturnFeedback_t DumpRegister_CoreAPI(std::vector<RegisterItem_t> &regs) override {
        return MEMU_OK;
    }

    FuncReturnFeedback_t Step_CoreAPI() override {
        cemuCore->step(meip, msip, mtip, seip);
        return MEMU_OK;
    }

    FuncReturnFeedback_t DumpProgramCounter_CoreAPI(RegisterItem_t &reg) override {
        assert(0);  // Not implemented
        return MEMU_OK;
    }

    FuncReturnFeedback_t WriteProgramCounter_CoreAPI(RegItemVal_t reg) override {
        cemuCore->jump(reg.u64_val);
        return MEMU_OK;
    }

    // ----- Constructor & Destructor
    CEmuCoreAdapter(MMIOBus_I* memu_bus, uint16_t hart_id) : busAdapter(memu_bus) {
        this->intStatus = new IntStatus_t;
        intStatus->mtip = false;
        intStatus->msip = false;
        intStatus->meip = false;
        intStatus->seip = false;
        this->cemuCore = new RVCore(busAdapter, (uint8_t)hart_id);
    }
    // ----- Member functions

    FuncReturnFeedback_t setGPR(uint8_t gpr_index, int64_t val) {
        cemuCore->set_GPR(gpr_index, val);
        return MEMU_OK;
    }

};