#pragma once

#include <vector>
#include "misc/status_enum.h"
#include "interface/Core.h"

struct IntStatus;
typedef IntStatus IntStatus_t;

class IntCtrl_I : public MMIODev_I {
    // ----- Fields
private:
    std::vector<MMIODev_I*> devArr;
    // ----- Interfaces
public:
    virtual FuncReturnFeedback_e SetCoreState_IntCtrl_API() = 0;
    virtual FuncReturnFeedback_e RegisterDev_IntCtrl_API(uint32_t int_id, MMIODev_I* dev) = 0;
    virtual FuncReturnFeedback_e UnregisterDev_IntCtrl_API(uint32_t int_id) = 0;
    virtual FuncReturnFeedback_e UnregisterDev_IntCtrl_API(MMIODev_I* dev) = 0;
    virtual FuncReturnFeedback_e UpdateIntState_IntCtrl_API() = 0;
    // ----- Constructor & Destructor
    IntCtrl_I(uint64_t devBaseAddr, uint64_t memRegionSize) :
        MMIODev_I(devBaseAddr, memRegionSize) {}
    // ----- Member functions
};
