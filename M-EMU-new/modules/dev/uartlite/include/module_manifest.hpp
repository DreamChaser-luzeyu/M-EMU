#pragma once

#include "sdk/instance_desc.h"
#include "interface/MMIO_Dev.h"
#include "MMIODev_CEmuAdapter.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: Do not define InstanceDesc_GV as constant.
InstanceDesc_t InstanceDesc_GV = {
        .desc_str = "uart_lite"
};

MMIODev_I *GetInstance_API() {
    return new CemuDevAdapter(new uartlite, 1024 * 1024);
}

#ifdef __cplusplus
}
#endif