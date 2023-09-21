#pragma once

#include "interface/MMU.h"

enum VAddr_RW_Feedback : uint16_t {
    TTT,WWW
};



class RV64SV39_MMU : public MMU_I {

    // ----- Interface implementation
public:
    VAddr_RW_Feedback_e PAddr_ReadBuffer_MMU_API(uint64_t begin_addr, uint64_t size, uint8_t *buffer) override;
    VAddr_RW_Feedback_e PAddr_WriteBuffer_MMU_API(uint64_t begin_addr, uint64_t size, uint8_t *buffer) override;


};




