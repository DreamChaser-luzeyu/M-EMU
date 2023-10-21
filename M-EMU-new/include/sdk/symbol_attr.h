#pragma once

#include "sdkconfig.h"

#if CONFIG_TEST_ENABLE
#define MEMU_STATIC
#else
#define MEMU_STATIC static
#endif