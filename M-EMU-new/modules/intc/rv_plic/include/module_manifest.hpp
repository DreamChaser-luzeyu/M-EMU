#pragma once

#include "sdk/instance_desc.h"
#include "interface/IntCtrl.h"
#include "PLICIntCtrl_CEmuAdapter.h"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: Do not define InstanceDesc_GV as constant.
InstanceDesc_t InstanceDesc_GV = {
        .desc_str = "rv_plic"
};

IntCtrl_I *GetInstance_API() {
    return new CemuPlicAdapter(new RVPLICtrl<4,4>, 0x4000000);
}

#ifdef __cplusplus
}
#endif