#pragma once

typedef enum {
    MEMU_OK = 0,
    MEMU_INVALID_PARAM,
    MEMU_NULL_PTR,
    MEMU_ADDR_CONFLICT,
    MEMU_INTNUM_CONFLICT,
    MEMU_INVALID_STATE,
    MEMU_NOT_FOUND,
    MEMU_NOT_ACCESSIBLE,
    MEMU_DATA_MODIFIED,
    MEMU_FILE_OPEN_ERR,
    MEMU_TIMEOUT,
    MEMU_ALIGNMENT_ERROR,
    MEMU_INVALID_ADDR,          ///< The address (in the emulator) is invalid
    MEMU_PANIC,
    MEMU_UNKNOWN,
    MEMU_NOT_IMPL,
    MEMU_PROTOCOL_ERR,
    MEMU_CHKSUM_ERR,
    MEMU_MODULE_MISSING_ENV
} FuncReturnFeedback_e;
