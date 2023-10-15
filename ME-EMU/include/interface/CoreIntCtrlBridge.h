#pragma once

#include "misc/status_enum.h"

struct IntStatus;
typedef IntStatus IntStatus_t;

// Useless?
class CoreIntCtrlBridge {
    // ----- Fields
    std::vector<IntCtrl_I*>       intCtrlArr;
    std::vector<ProcessorCore_I*> coreArr;
    // ----- Interface implementation
    // ----- Constructor & Destructor

    // ----- Member functions
    FuncReturnFeedback_e registerCore(ProcessorCore_I* core) {
        // TODO: Check if obj already exists
        
        return MEMU_OK;
    }

    FuncReturnFeedback_e registerIntCtrl(IntCtrl_I* intCtrl) {

        return MEMU_OK;
    }

    FuncReturnFeedback_e updateState() {

        return MEMU_OK;
    }

};