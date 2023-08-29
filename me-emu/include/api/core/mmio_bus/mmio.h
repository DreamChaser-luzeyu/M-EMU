#ifndef ME_EMU_MMIO_H
#define ME_EMU_MMIO_H
#include "struct/misc/status_enum.h"

typedef struct {
    uint64_t begin_address;
    uint64_t end_address;
} AddressRegion_t;

/**
 * @brief Read data from physical memory
 * @param begin_addr Begin address
 * @param size (In byte) Size of data
 * @param[out] buffer Buffer to dump the data
 * @return Read feedback
 *      - MEMU_OK
 *      - MEMU_INVALID_ADDR
 *      - MEMU_ALIGNMENT_ERROR
 */
FuncReturnFeedback_t ReadBuffer_MMIO_Bus_API(uint64_t begin_addr, size_t size, uint8_t* buffer);

/**
 * @brief Write data from physical memory
 * @param begin_addr Begin address
 * @param size (In byte) Size of data
 * @param[out] buffer Buffer of the data
 * @return Write feedback
 *      - MEMU_OK
 *      - MEMU_INVALID_ADDR
 *      - MEMU_ALIGNMENT_ERROR
 */
FuncReturnFeedback_t WriteBuffer_MMIO_Bus_API(uint64_t begin_addr, size_t size, uint8_t* buffer);

#endif //ME_EMU_MMIO_H
