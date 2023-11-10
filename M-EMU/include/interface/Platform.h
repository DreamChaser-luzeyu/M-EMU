#pragma once


#include "misc/status_enum.h"

class Platform_I {
public:

    virtual FuncReturnFeedback_e Init_Platform_API() { return MEMU_OK; }

    /**
     * This function should never return while the guest running.
     * @return
     */
    virtual FuncReturnFeedback_e Run_Platform_API() = 0;

    /**
     *
     * @return
     */
    virtual FuncReturnFeedback_e WaitForDebugger_Platform_API() = 0;
};
