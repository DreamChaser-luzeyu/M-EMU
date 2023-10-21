#pragma once

#include "sdk/instance_desc.h"
#include "interface/IntCtrl.h"
#include "ClintIntCtrl_CEmuAdapter.h"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: Do not define InstanceDesc_GV as constant.
InstanceDesc_t InstanceDesc_GV = {
        .desc_str = "rv_clint"
};

IntCtrl_I *GetInstance_API() {
    return new CemuClintAdapter(new RVCoreLocalInt<2>, 0x10000);
}

#ifdef __cplusplus
}
#endif