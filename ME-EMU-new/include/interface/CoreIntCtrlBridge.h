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
    FuncReturnFeedback_t registerCore(ProcessorCore_I* core) {
        // TODO: Check if obj already exists
        
        return MEMU_OK;
    }

    FuncReturnFeedback_t registerIntCtrl(IntCtrl_I* intCtrl) {

        return MEMU_OK;
    }

    FuncReturnFeedback_t updateState() {

        return MEMU_OK;
    }

};