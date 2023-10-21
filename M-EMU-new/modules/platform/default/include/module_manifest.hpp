#pragma once

#include "sdk/instance_desc.h"
#include "DefaultPlatform.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: Do not define InstanceDesc_GV as constant. 
InstanceDesc_t InstanceDesc_GV = {
        .desc_str = "default_platform"
};

Platform_I* GetInstance_API() {
    return new DefaultPlatform();
}

#ifdef __cplusplus
}
#endif