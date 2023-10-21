#pragma once

#include "sdk/instance_desc.h"
#include "interface/MMIO_Bus.h"
#include "MMIOBus_Impl.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: Do not define InstanceDesc_GV as constant. 
InstanceDesc_t InstanceDesc_GV = {
        .desc_str = "mmio_bus"
};

MMIOBus_I* GetInstance_API() {
    return new MEmu_MMIOBus();
}

#ifdef __cplusplus
}
#endif