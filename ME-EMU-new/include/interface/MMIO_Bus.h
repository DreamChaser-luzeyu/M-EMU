#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include "interface/MMIO_Dev.h"
#include "misc/status_enum.h"

class MMIOBus_I {
private:
    uint64_t preservedBaseAddr;
    uint8_t* preservedDataBuffer;
    uint16_t preserverHartID;
    uint64_t preservedSize;       ///< 0 means no preserved region

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
    virtual FuncReturnFeedback_t PAddr_ReadBuffer_MMIOBus_API(uint64_t begin_addr, uint64_t size, uint8_t* buffer) = 0;

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
    virtual FuncReturnFeedback_t PAddr_WriteBuffer_MMIOBus_API(uint64_t begin_addr, uint64_t size, const uint8_t* buffer) = 0;

    /**
     *
     * @param regions
     * @param size
     * @return
     */
    virtual FuncReturnFeedback_t RegisterMMIODev_MMIOBus_API(MMIODev_I* dev, uint64_t begin_addr) = 0;

    /**
     * Mark address region [begin_addr, begin_addr + size] as `preserved` for atomic memory operation
     * @param begin_addr
     * @param size
     * @param buffer
     * @return
     */
    FuncReturnFeedback_t PAddr_PreserveAndReadBuffer_MMIOBus_API(uint16_t preserver_hart_id, uint64_t begin_addr,
                                                                 uint64_t size, uint8_t* buffer) {
        this->preservedBaseAddr = begin_addr;
        this->preservedSize = size;
        this->preserverHartID = preserver_hart_id;
        if(this->preservedDataBuffer) { delete this->preservedDataBuffer; }
        this->preservedDataBuffer = new uint8_t[size];
        // Read data to preservedDataBuffer for further compare
        PAddr_ReadBuffer_MMIOBus_API(begin_addr, size, this->preservedDataBuffer);
        // Do read
        return PAddr_ReadBuffer_MMIOBus_API(begin_addr, size, buffer);
    }

    FuncReturnFeedback_t PAddr_CheckExclusiveAndWriteBuffer_MMIOBus_API(uint16_t modifier_hart_id, uint8_t* buffer) {
        // Calling without marking any address region reserved
        if(!(this->preservedSize)) { return MEMU_INVALID_STATE; }
        assert(this->preservedDataBuffer);
        assert(this->preserverHartID == modifier_hart_id);  // TODO: Replace this with self-implemented panic
        // --- Check if memory region modified by other hart
        uint8_t* current_preserved_data = new uint8_t[this->preservedSize];
        PAddr_ReadBuffer_MMIOBus_API(this->preservedBaseAddr, this->preservedSize, current_preserved_data);
        int preserved_data_modified = memcmp(this->preservedDataBuffer, current_preserved_data, this->preservedSize);
        // Address region modified by other hart, abort write
        if(preserved_data_modified != 0) { return MEMU_DATA_MODIFIED; }
        // Address region unmodified, do write buffer
        return PAddr_WriteBuffer_MMIOBus_API(this->preservedBaseAddr, this->preservedSize, buffer);
    }

    // ----- Constructor & Destructor
protected:
    MMIOBus_I() : preservedBaseAddr(0), preservedSize(0), preservedDataBuffer(nullptr), preserverHartID(0) {}

};