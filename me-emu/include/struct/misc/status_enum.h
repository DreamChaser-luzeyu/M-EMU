//
// Created by luzeyu on 23-8-29.
//

#ifndef ME_EMU_STATUS_ENUM_H
#define ME_EMU_STATUS_ENUM_H

typedef enum {
    MEMU_OK = 0,
    MEMU_INVALID_PARAM,
    MEMU_INVALID_STATE,
    MEMU_TIMEOUT,
    MEMU_ALIGNMENT_ERROR,
    MEMU_INVALID_ADDR,          ///< The address (in the emulator) is invalid
    MEMU_PANIC,
    MEMU_UNKNOWN
} FuncReturnFeedback_t;

#endif //ME_EMU_STATUS_ENUM_H
