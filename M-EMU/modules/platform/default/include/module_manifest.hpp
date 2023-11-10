#pragma once

#include "sdk/instance_desc.h"
#include "DefaultPlatform.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: Do not define InstanceDesc_GV as constant. 
InstanceDesc_t InstanceDesc_GV = {
        .desc_str = "default_platform",
        .env_help_list = nullptr,
        .env_help_list_len = 0,
};

Platform_I* GetInstance_API() {
    return new DefaultPlatform();
}

#ifdef __cplusplus
}
#endif