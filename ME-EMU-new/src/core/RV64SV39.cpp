#include "custom/core/RV64SV39_MMU.h"

VAddr_RW_Feedback_e RV64SV39_MMU::PAddr_ReadBuffer_MMU_API(uint64_t begin_addr, uint64_t size, uint8_t *buffer)
{
    VAddr_RW_Feedback_e result = WWW;
    return result;
}

VAddr_RW_Feedback_e RV64SV39_MMU::PAddr_WriteBuffer_MMU_API(uint64_t begin_addr, uint64_t size, uint8_t *buffer)
{
    VAddr_RW_Feedback_e result = TTT;
    return result;
}
