#pragma once

#include <vector>
#include "misc/status_enum.h"
#include "interface/Core.h"
#include "interface/MMIO_Dev.h"

struct IntStatus;
typedef IntStatus IntStatus_t;

class IntCtrl_I : public MMIODev_I {
    // ----- Fields
private:
    std::vector<MMIODev_I*> devArr;
protected:
    std::vector<IntStatus_t*> intStatus;
    // ----- Interfaces
public:
    virtual FuncReturnFeedback_e SetCoreState_IntCtrl_API() = 0;
    virtual FuncReturnFeedback_e RegisterDev_IntCtrl_API(uint32_t int_id, MMIODev_I* dev) = 0;
    virtual FuncReturnFeedback_e UnregisterDev_IntCtrl_API(uint32_t int_id) = 0;
    virtual FuncReturnFeedback_e UnregisterDev_IntCtrl_API(MMIODev_I* dev) = 0;
    virtual FuncReturnFeedback_e UpdateIntState_IntCtrl_API() = 0;
    // ----- Constructor & Destructor
    IntCtrl_I(uint64_t devBaseAddr, uint64_t memRegionSize) :
        MMIODev_I(devBaseAddr, memRegionSize) { intStatus.resize(10); }
    // ----- Member functions
    virtual FuncReturnFeedback_e AttachCoreIntStatus(IntStatus_t* int_status, uint64_t hart_id) {
//        if((intStatus.size() - 1) < hart_id) intStatus.resize(hart_id + 1);
        intStatus[hart_id] = int_status;
        return MEMU_OK;
    }

};
