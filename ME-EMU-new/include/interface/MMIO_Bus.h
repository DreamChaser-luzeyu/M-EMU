#pragma once
#include <cstdint>
#include <cstddef>
#include "interface/MMIO_Dev.h"
#include "misc/status_enum.h"

class MMIOBus_I {
public:

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
    virtual FuncReturnFeedback_t PAddr_ReadBuffer_MMIOBus_API (uint64_t begin_addr, size_t size, uint8_t* buffer) = 0;

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
    virtual FuncReturnFeedback_t PAddr_WriteBuffer_MMIOBus_API(uint64_t begin_addr, size_t size, uint8_t* buffer) = 0;

    /**
     *
     * @param regions
     * @param size
     * @return
     */
    virtual FuncReturnFeedback_t RegisterMMIODev_MMIOBus_API(MMIODev_I* dev, uint64_t begin_addr) = 0;
};