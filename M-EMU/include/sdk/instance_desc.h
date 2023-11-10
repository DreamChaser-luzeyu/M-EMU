#pragma once

#include <stdint.h>

typedef struct {
    const char* env_key;
    const char* env_help;
} EnvConfHelp_t;

typedef struct {
    const char*  desc_str;
    const EnvConfHelp_t* env_help_list;
    const uint32_t env_help_list_len;
} InstanceDesc_t;
