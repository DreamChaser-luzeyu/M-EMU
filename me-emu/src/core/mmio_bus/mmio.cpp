//
// Created by luzeyu on 23-8-30.
//
#include <cstddef>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstring>
#include "api/core/mmio_bus/mmio.h"
using std::vector;

vector<AddressRegion_t> DevAddrRegions_GV;
extern AddressRegion_t RAM_AddrRegion_GV;       ///< Cache RAM addr region for better performance

FuncReturnFeedback_t RegisterAddrRegionForDev_MMIO_Bus_API(const AddressRegion_t& region) {
    // Check if address regions conflict
    // TODO: use a better algorithm for better performance
    for(AddressRegion_t ar : DevAddrRegions_GV) { assert(!is_addr_regions_conflict(ar, region)); }

    DevAddrRegions_GV.push_back(region);
    std::sort(DevAddrRegions_GV.begin(), DevAddrRegions_GV.end(),
              [](const AddressRegion_t& o1, const AddressRegion_t& o2){
        return o1.begin_address < o2.begin_address;
    });
    return MEMU_OK;
}

FuncReturnFeedback_t ReadBuffer_MMIO_Bus_API(uint64_t begin_addr, size_t size, uint8_t* buffer) {
    // TODO: Optimize with `likely`
    if(!buffer) { return MEMU_INVALID_PARAM; }

    // Check if in RAM region FIRST for better performance
    // TODO: Optimize with `likely`
    if(is_addr_in_region(begin_addr, RAM_AddrRegion_GV)) {
        // TODO: Optimize with `likely` since RAM always prefer raw_addr
        if(RAM_AddrRegion_GV.raw_addr) {
            RAM_AddrRegion_GV.dev_read(begin_addr, size, buffer);
        }
        // In our case, RAM accepts raw physical addr.
        else { assert(0); }
        return MEMU_OK;
    }

    // TODO: Find the dev which `begin_addr` matches and do read

    return MEMU_OK;
}

FuncReturnFeedback_t WriteBuffer_MMIO_Bus_API(uint64_t begin_addr, size_t size, uint8_t* buffer) {
    // TODO: Optimize with `likely`
    if(!buffer) { return MEMU_INVALID_PARAM; }

    // Check if in RAM region FIRST for better performance
    // TODO: Optimize with `likely`
    if(is_addr_in_region(begin_addr, RAM_AddrRegion_GV)) {
        if(RAM_AddrRegion_GV.raw_addr) {
            RAM_AddrRegion_GV.dev_write(begin_addr, size, buffer);
        }
        else { assert(0); }
        return MEMU_OK;
    }

    // TODO

    return MEMU_OK;
}