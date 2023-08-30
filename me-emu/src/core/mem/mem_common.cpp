//
// Created by luzeyu on 23-8-30.
//
#include <stdint.h>
#include <cstring>
#include "api/core/mmio_bus/mmio.h"

AddressRegion_t RAM_AddrRegion_GV;
uint8_t* PhysicalRAM_GV;


static inline FuncReturnFeedback_t ReadBuffer_PhysicalMemAPI (uint64_t begin_addr, size_t size, uint8_t* buffer);
static inline FuncReturnFeedback_t WriteBuffer_PhysicalMemAPI(uint64_t begin_addr, size_t size, uint8_t* buffer);

FuncReturnFeedback_t InitRAM(uint64_t size, uint64_t begin_addr) {
    PhysicalRAM_GV = new uint8_t[size];

    RAM_AddrRegion_GV.dev_read  = ReadBuffer_PhysicalMemAPI;
    RAM_AddrRegion_GV.dev_write = WriteBuffer_PhysicalMemAPI;
    RAM_AddrRegion_GV.begin_address = begin_addr;
    RAM_AddrRegion_GV.size = size;
    RAM_AddrRegion_GV.raw_addr = true;

    return MEMU_OK;
}

static inline FuncReturnFeedback_t ReadBuffer_PhysicalMemAPI(uint64_t begin_addr, size_t size, uint8_t* buffer) {
    if(!buffer) { return MEMU_INVALID_PARAM; }
    if(begin_addr < RAM_AddrRegion_GV.begin_address) { return MEMU_INVALID_PARAM; }

    uint64_t begin_index = begin_addr - RAM_AddrRegion_GV.begin_address;
    memcpy(buffer, &((PhysicalRAM_GV)[begin_index]), size);

    return MEMU_OK;
}

static inline FuncReturnFeedback_t WriteBuffer_PhysicalMemAPI(uint64_t begin_addr, size_t size, uint8_t* buffer) {
    if(!buffer) { return MEMU_INVALID_PARAM; }
    if(begin_addr < RAM_AddrRegion_GV.begin_address) { return MEMU_INVALID_PARAM; }

    uint64_t begin_index = begin_addr - RAM_AddrRegion_GV.begin_address;
    memcpy(&((PhysicalRAM_GV)[begin_index]), buffer, size);

    return MEMU_OK;
}
