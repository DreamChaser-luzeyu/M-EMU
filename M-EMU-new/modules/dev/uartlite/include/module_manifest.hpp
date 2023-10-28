#pragma once

#include "sdk/instance_desc.h"
#include "interface/MMIO_Dev.h"
#include "MMIODev_CemuUartAdapter.hpp"

#ifdef __cplusplus
extern "C" {
#endif

const static EnvConfHelp_t env_helps[] = {
        { .env_key = TTY_DEV_ENV_KEY, .env_help = "You need to create a pair of tty vdev to use this peripheral.\n"
                                                  "You could either use `socat` command or other methods to do so.\n"
                                                  "Please specify path to one of the tty pair." }
};

// NOTE: Do not define InstanceDesc_GV as constant.
InstanceDesc_t InstanceDesc_GV = {
        .desc_str = "uart_lite",
        .env_help_list = env_helps,
        .env_help_list_len = 1,
};

MMIODev_I *GetInstance_API() {
    return new CemuUartAdapter(new uartlite, 1024 * 1024);
}

#ifdef __cplusplus
}
#endif