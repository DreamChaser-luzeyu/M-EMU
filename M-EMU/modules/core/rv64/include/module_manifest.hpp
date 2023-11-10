#pragma once

#include "sdk/instance_desc.h"
#include "interface/MMIO_Bus.h"
#include "RV64Core.h"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: Do not define InstanceDesc_GV as constant. 
InstanceDesc_t InstanceDesc_GV = {
        .desc_str = "rv64core_simple",
        .env_help_list = nullptr,
        .env_help_list_len = 0,
};

ProcessorCore_I* GetInstance_API(MMIOBus_I* sys_bus, uint64_t hart_id) {
    return new RV64Core(sys_bus, hart_id);
}

#ifdef __cplusplus
}
#endif