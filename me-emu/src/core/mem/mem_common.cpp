//
// Created by luzeyu on 23-8-30.
//
#include <stdint.h>
#include <cstring>
#include "api/core/mmio_bus/mmio.h"

uint8_t* PhysicalRAM_GV;
uint64_t PhysicalRAM_BeginAddr_GV;
uint64_t PhysicalRAM_Size_GV;

FuncReturnFeedback_t InitRAM(uint64_t size, uint64_t begin_addr) {
    PhysicalRAM_GV = new uint8_t[size];
    PhysicalRAM_BeginAddr_GV = begin_addr;
    PhysicalRAM_Size_GV = size;
    return MEMU_OK;
}

FuncReturnFeedback_t ReadBuffer_PhysicalMemAPI(uint64_t begin_addr, size_t size, uint8_t* buffer) {
    if(!buffer) { return MEMU_INVALID_PARAM; }
    if(begin_addr < PhysicalRAM_BeginAddr_GV) { return MEMU_INVALID_PARAM; }

    uint64_t begin_index = begin_addr - PhysicalRAM_BeginAddr_GV;
    memcpy(buffer, &(PhysicalRAM_GV[begin_index]), size);

    return MEMU_OK;
}

FuncReturnFeedback_t WriteBuffer_PhysicalMemAPI(uint64_t begin_addr, size_t size, uint8_t* buffer) {
    if(!buffer) { return MEMU_INVALID_PARAM; }
    if(begin_addr < PhysicalRAM_BeginAddr_GV) { return MEMU_INVALID_PARAM; }

    uint64_t begin_index = begin_addr - PhysicalRAM_BeginAddr_GV;
    memcpy(&(PhysicalRAM_GV[begin_index]), buffer, size);

    return MEMU_OK;
}
