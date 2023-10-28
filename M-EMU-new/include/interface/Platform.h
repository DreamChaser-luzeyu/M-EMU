#pragma once

#include "misc/status_enum.h"

class Platform_I {
public:
    virtual FuncReturnFeedback_e Init_Platform_API() { return MEMU_OK; }
    virtual FuncReturnFeedback_e Run_Platform_API() = 0;
    virtual FuncReturnFeedback_e WaitForDebugger_Platform_API() = 0;
};
