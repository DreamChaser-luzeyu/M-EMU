// For user implementation
#ifndef ME_EMU_PAGING_H
#define ME_EMU_PAGING_H
#include "struct/misc/status_enum.h"

/**
 * @brief Convert physical address to virtual address
 * @param[in] paddr Physical address
 * @param[out] vaddr (Pointer to) Virtual address
 * @return Conversion feedback
 *      - MEMU_OK
 *      - MEMU_INVALID_ADDR
 *      - MEMU_ALIGNMENT_ERROR
 */
FuncReturnFeedback_t PAddr_to_VAddr_PagingAPI(uint64_t paddr, uint64_t* vaddr);

#endif //ME_EMU_PAGING_H
