#ifndef ME_EMU_MMIO_H
#define ME_EMU_MMIO_H
#include <cstdint>
#include "struct/misc/status_enum.h"

// TODO: Warp with class MMIO_Device
typedef struct {
    FuncReturnFeedback_t (*dev_read)(uint64_t, size_t, uint8_t*);
    FuncReturnFeedback_t (*dev_write)(uint64_t, size_t, uint8_t*);
    uint64_t begin_address;
    size_t size;
    bool raw_addr;
} AddressRegion_t;

static inline bool is_addr_regions_conflict(const AddressRegion_t& ar1, const AddressRegion_t& ar2) {
    uint64_t ar1_begin_addr = ar1.begin_address;
    uint64_t ar1_end_addr   = ar1_begin_addr + ar1.size;
    uint64_t ar2_begin_addr = ar2.begin_address;
    uint64_t ar2_end_addr   = ar2_begin_addr + ar2.size;
    if(ar1_begin_addr <= ar2_begin_addr && ar2_begin_addr <= ar1_end_addr) { return true; }
    if(ar1_begin_addr <= ar2_end_addr && ar2_end_addr <= ar1_end_addr) { return true; }
    return false;
}

static inline bool is_addr_in_region(uint64_t addr, const AddressRegion_t& region) {
    return (region.begin_address <= addr) && (addr <= (region.begin_address + region.size));
}


/**
 *
 * @param regions
 * @param size
 * @return
 */
FuncReturnFeedback_t RegisterAddrRegionForDev_MMIO_Bus_API(const AddressRegion_t& region);

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
