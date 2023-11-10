#pragma once

#include "sdk/instance_desc.h"
#include "interface/MMIO_Dev.h"
#include "MMIOMem_Impl.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: Do not define InstanceDesc_GV as constant.
InstanceDesc_t InstanceDesc_GV = {
        .desc_str = "simple_ram",
        .env_help_list = nullptr,
        .env_help_list_len = 0,
};

MMIODev_I *GetInstance_API() {
    return new MEmu_MMIO_Mem(1024 * 1024 * 4096l);
}

#ifdef __cplusplus
}
#endif