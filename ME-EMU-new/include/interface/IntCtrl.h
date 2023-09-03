#pragma once

#include <vector>
#include "misc/status_enum.h"
#include "interface/Processor.h"

class IntCtrl_I : public MMIODev_I {
    // ----- Fields
private:
    std::vector<MMIODev_I*> devArr;
    // ----- Interfaces
public:
    virtual FuncReturnFeedback_t SetCoreState_IntCtrl_API() = 0;
    virtual FuncReturnFeedback_t RegisterDev_IntCtrl_API(uint32_t int_id, MMIODev_I* dev) = 0;
    virtual FuncReturnFeedback_t UnregisterDev_IntCtrl_API(uint32_t int_id) = 0;
    virtual FuncReturnFeedback_t UnregisterDev_IntCtrl_API(MMIODev_I* dev) = 0;
    virtual FuncReturnFeedback_t UpdateIntState_IntCtrl_API() = 0;
    // ----- Constructor & Destructor
    IntCtrl_I(uint64_t devBaseAddr, uint64_t memRegionSize) : MMIODev_I(devBaseAddr, memRegionSize) {}
    // ----- Member functions

};
