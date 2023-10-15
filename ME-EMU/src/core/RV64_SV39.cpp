#include "custom/core/RV64SV39_MMU.h"

VAddr_RW_Feedback_e RV64SV39_MMU::VAddr_ReadBuffer_MMU_API(uint64_t begin_addr, uint64_t size, uint8_t *buffer) {
    const SATP_Reg_t* satp_reg = (SATP_Reg_t*)&satp;
    const CSReg_MStatus_t* mstatus = (CSReg_MStatus_t*)&status;
    // --- No need to do page-based vaddr converting
    if( (currentPrivMode == M_MODE && (!mstatus->mprv || mstatus->mpp == M_MODE))
     || (satp_reg->mode == 0) ) {
        FuncReturnFeedback_e pread_fb = sysBus->PAddr_ReadBuffer_MMIOBus_API(begin_addr, size, buffer);
        if(pread_fb != MEMU_OK) { return VADDR_ACCESS_FAULT; }
        return VADDR_ACCESS_OK;
    }
    // ----- Need page-based vaddr converting
    // --- Check if data within page region
    if ((begin_addr >> 12) != ((begin_addr + size - 1) >> 12)) {
        // Out of page range, cross-page access
        return VADDR_MIS_ALIGN;
    }
    // --- Get & Check TLB Entry
    SV39_TLB_Entry_t* tlb_entry = getTLBEntry({.val = begin_addr});
    if (!tlb_entry || !tlb_entry->A || (!tlb_entry->R && !(mstatus->mxr && !tlb_entry->X))) {
        return VADDR_PAGE_FAULT;
    }
    PrivMode_e priv = (mstatus->mprv && currentPrivMode == M_MODE) ? (PrivMode_e)(mstatus->mpp) : currentPrivMode;
    if (priv == U_MODE && !tlb_entry->U) { return VADDR_PAGE_FAULT; }
    if (!mstatus->sum && currentPrivMode == S_MODE && tlb_entry->U) {
        return VADDR_ACCESS_FAULT;
    }
    // --- Get physical address from tlb_entry
    uint64_t page_size = (tlb_entry->pagesize == 1) ? (1 << 12) :
                         (tlb_entry->pagesize == 2) ? (1 << 21) :
                         (tlb_entry->pagesize == 3) ? (1 << 30) : 0;
    assert(page_size);
    uint64_t paddr = tlb_entry->ppa + (begin_addr % page_size);
    // Do read/write
    FuncReturnFeedback_e pread_fb = sysBus->PAddr_ReadBuffer_MMIOBus_API(paddr, size, buffer);
    if (pread_fb != MEMU_OK) { return VADDR_ACCESS_FAULT; }
    return VADDR_ACCESS_OK;
}

VAddr_RW_Feedback_e RV64SV39_MMU::VAddr_InstFetch_MMU_API(uint64_t begin_addr, uint64_t size, void* buffer) {
    assert(begin_addr % 2 == 0);
    if(size == 4 && begin_addr % 4 == 2) {
        // A 4-byte instruction could be placed at different pages, but we do not want to throw page fault
        // in such condition. We could split the inst-fetch operation into 2 parts, first we fetch the low
        // 2 bytes, then we fetch the high 2 bytes.
        VAddr_RW_Feedback_e fb_1 = VAddr_InstFetch_MMU_API(begin_addr, 2, buffer);
        if(fb_1 != VADDR_ACCESS_OK) { return fb_1; }
        VAddr_RW_Feedback_e fb_2 = VAddr_InstFetch_MMU_API(begin_addr + 2, 2, ((uint8_t*)buffer) + 2);
        if(fb_2 != VADDR_ACCESS_OK) { return fb_2; }
        return VADDR_ACCESS_OK;
    }
    const SATP_Reg_t *satp_reg = (SATP_Reg_t *)&satp;
    // --- No need to do page-based vaddr converting
    if (currentPrivMode == M_MODE || satp_reg->mode == 0) {
        FuncReturnFeedback_e pstatus = sysBus->PAddr_ReadBuffer_MMIOBus_API(begin_addr, size, (uint8_t*)buffer);
        if (pstatus != MEMU_OK) { return INST_ACCESS_FAULT; }
        return INST_ACCESS_OK;
    }
    // ----- Need page-based vaddr converting
//    assert(0);
    // --- Check if data within page region
    if ((begin_addr >> 12) != ((begin_addr + size - 1) >> 12)) {
        // Out of page range, cross-page access
        return VADDR_MIS_ALIGN;
    }
    // --- Get & Check TLB Entry
    SV39_TLB_Entry_t* tlb_entry = getTLBEntry({.val = begin_addr});
    if (!tlb_entry || !tlb_entry->A || !tlb_entry->X) { return INST_PAGE_FAULT; }
    if ( (currentPrivMode == U_MODE && !tlb_entry->U)
      || (currentPrivMode == S_MODE && tlb_entry->U) ) {
        return INST_PAGE_FAULT;
    }
    // --- Get physical address from tlb_entry
    uint64_t page_size = (tlb_entry->pagesize == 1) ? (1 << 12) :
                         (tlb_entry->pagesize == 2) ? (1 << 21) :
                         (tlb_entry->pagesize == 3) ? (1 << 30) : 0;
    assert(page_size);
    uint64_t paddr = tlb_entry->ppa + (begin_addr % page_size);
    // Do read/write
    FuncReturnFeedback_e pstatus = sysBus->PAddr_ReadBuffer_MMIOBus_API(paddr, size, (uint8_t*)buffer);
    if (pstatus != MEMU_OK) { return INST_ACCESS_FAULT; }
    return INST_ACCESS_OK;
}

VAddr_RW_Feedback_e RV64SV39_MMU::VAddr_WriteBuffer_MMU_API(uint64_t begin_addr, uint64_t size, const uint8_t *buffer) {
    const SATP_Reg_t* satp_reg = (SATP_Reg_t*)&satp;
    const CSReg_MStatus_t* mstatus = (CSReg_MStatus_t*)&status;
    // --- No need to do page-based vaddr converting
    if ( (currentPrivMode == M_MODE && (!mstatus->mprv || mstatus->mpp == M_MODE))
      || (satp_reg->mode == 0) ) {
        FuncReturnFeedback_e pwrite_fb = sysBus->PAddr_WriteBuffer_MMIOBus_API(begin_addr,size,buffer);
        if (pwrite_fb != MEMU_OK) { return VADDR_ACCESS_FAULT; }
        return VADDR_ACCESS_OK;
    }
    // ----- Need page-based vaddr converting
    // --- Check if data within page region
    if ((begin_addr >> 12) != ((begin_addr + size - 1) >> 12)) {
        // Out of page range, cross-page access
        return VADDR_MIS_ALIGN;
    }
    // --- Get & Check TLB Entry
    SV39_TLB_Entry_t* tlb_entry = getTLBEntry({.val = begin_addr});
    if (!tlb_entry || !tlb_entry->A || !tlb_entry->D || !tlb_entry->W) {
        return VADDR_PAGE_FAULT;
    }
    PrivMode_e priv = (mstatus->mprv && currentPrivMode == M_MODE) ? (PrivMode_e)(mstatus->mpp) : currentPrivMode;
    if (priv == U_MODE && !tlb_entry->U) { return VADDR_PAGE_FAULT; }
    if (!mstatus->sum && priv == S_MODE && tlb_entry->U) { return VADDR_PAGE_FAULT; }
    // --- Get physical address from tlb_entry
    uint64_t page_size = (tlb_entry->pagesize == 1) ? (1 << 12) :
                         (tlb_entry->pagesize == 2) ? (1 << 21) :
                         (tlb_entry->pagesize == 3) ? (1 << 30) : 0;
    assert(page_size);
    uint64_t paddr = tlb_entry->ppa + (begin_addr % page_size);
    // Do read/write
    FuncReturnFeedback_e pstatus = sysBus->PAddr_WriteBuffer_MMIOBus_API(paddr,size,buffer);
    if (pstatus != MEMU_OK) { return VADDR_PAGE_FAULT; }
    return VADDR_ACCESS_OK;
}

void RV64SV39_MMU::SV39_FlushTLB_sfence_vma(uint64_t vaddr, uint64_t asid) {
    for (int i=0;i<32;i++) {
        if (tlb[i].asid == asid || asid == 0) {
            if (vaddr == 0) tlb[i].pagesize = 0;
            else {
                switch (tlb[i].pagesize) {
                    case 1: // 4KB
                        if ((tlb[i].vpa & (-(1ll<<12))) == (vaddr & (-(1ll<<12))) ) tlb[i].pagesize = 0;
                        break;
                    case 2: // 2MB
                        if ((tlb[i].vpa & (-(1ll<<21))) == (vaddr & (-(1ll<<21))) ) tlb[i].pagesize = 0;
                        break;
                    case 3: // 1G
                        if ((tlb[i].vpa & (-(1ll<<30))) == (vaddr & (-(1ll<<30))) ) tlb[i].pagesize = 0;
                    default:
                        break;
                }
            }
        }
    }
}

bool RV64SV39_MMU::isValidDirPTE(const SV39_PageTableEntry_t& pte) {
    if(!pte.V) return false;                     // Current entry has to be VALID
    if(pte.R || pte.W || pte.X) return false;    // R W X bits all have to be ZERO for Page Dir Entry
    if(pte.reserved) return false;               // Reserved fiedls has to be ZERO
    if(pte.PBMT) return false;                   // TODO: Not implemented feature, to be replace with panic func
    if(!pte.R && !pte.W && !pte.X) return true;  // R W X bits all ZERO means a DIR PTE
    assert(0);
    //    return true;
}

bool RV64SV39_MMU::isValidLeafPTE(const SV39_PageTableEntry_t &pte) {
    if(!pte.V) return false;                     // Current entry has to be VALID
    if(pte.W && !pte.R) return false;            // Writable but not readable is INVALID
    if(!pte.R && !pte.W && !pte.X) return false; // R W X bits all ZERO means a DIR PTE, not a LEAF PTE
    if(pte.reserved) return false;               // Reserved fiedls has to be ZERO
    if(pte.PBMT) return false;                   // TODO: Not implemented feature, to be replaced with panic func
    if(pte.R || pte.X) return true;              // LEAF pte
    assert(0);
}

uint16_t RV64SV39_MMU::getTLBIndexToSwapOut() {
    uint16_t index = lastSwappedIndex;
    lastSwappedIndex = (lastSwappedIndex + 1) % 32;
    return index;
}

SV39_TLB_Entry_t * RV64SV39_MMU::writeTLB(const SV39_PageTableEntry_t& leaf_pte, uint64_t page_size,
                                          const SV39_VAddr_t& vaddr) {
    SV39_TLB_Entry_t* result = &(tlb[getTLBIndexToSwapOut()]);
    // Joint PAddr of physical page
    result->ppa = (((uint64_t)(leaf_pte.PPN2) << 18) | ((uint64_t)(leaf_pte.PPN1) << 9) | ((uint64_t)(leaf_pte.PPN0))) << 12;
    assert((result->ppa) == ((((((uint64_t)leaf_pte.PPN2 << 9) | (uint64_t)leaf_pte.PPN1) << 9) | (uint64_t)leaf_pte.PPN0) << 12));
    // Get VAddr of page
    uint64_t mask = page_size == 1 << 12 ? _4k_PAGE_ADDR_MASK :
                    page_size == 1 << 21 ? _2M_PAGE_ADDR_MASK :
                    page_size == 1 << 30 ? _1G_PAGE_ADDR_MASK : 0;
    assert(mask);
    result->vpa = vaddr.val & mask;
    assert((result->vpa) == (page_size == (1<<12)) ? (vaddr.val - (vaddr.val % (1<<12))) : (page_size == (1<<21)) ? (vaddr.val - (vaddr.val % (1<<21))) : (vaddr.val - (vaddr.val % (1<<30))));

    result->asid = satp.asid;
    result->pagesize = page_size == (1 << 12) ? 1 :
                       page_size == (1 << 21) ? 2 :
                       page_size == (1 << 30) ? 3 : 0;
    assert(result->pagesize);
    result->R = leaf_pte.R;
    result->W = leaf_pte.W;
    result->X = leaf_pte.X;
    result->U = leaf_pte.U;
    result->G = leaf_pte.G;
    result->A = leaf_pte.A;
    result->D = leaf_pte.D;

    // TODO: Self-test, to be removed
    if(getTLBEntry(vaddr) != result) assert(false);

    return result;
}

SV39_TLB_Entry_t* RV64SV39_MMU::getTLBEntry(SV39_VAddr_t vaddr) {
    // RISC-V ISA Regulation, blank field has to be the same as HSB of vpn_2
    assert((vaddr.blank == 0b1111111111111111111111111 && (vaddr.vpn_2 >> 8)) ||
           (vaddr.blank == 0 && ((vaddr.vpn_2 >> 8) == 0)));
    // ----- Try finding entry in TLB
    // TODO: Replace with HashMap for better performance
    SV39_TLB_Entry_t* tlb_entry_ptr = nullptr;
    for (int i = 0; i < 32; i++) {
        if (tlb[i].asid == satp.asid || tlb[i].G) {
            uint64_t page_addr_mask = tlb[i].pagesize == 1 ? _4k_PAGE_ADDR_MASK :
                                      tlb[i].pagesize == 2 ? _2M_PAGE_ADDR_MASK :
                                      tlb[i].pagesize == 3 ? _1G_PAGE_ADDR_MASK : 0;
            if(!page_addr_mask) continue;         // Maybe TLB not initialized
            if((vaddr.val & page_addr_mask) == tlb[i].vpa) {
                assert(tlb_entry_ptr == nullptr);
                tlb_entry_ptr = &tlb[i];
            }
        }
    }
    // Found TLB Entry, return directly
    if(tlb_entry_ptr) {
        return tlb_entry_ptr;
    }
    // ----- Find in Page Table
    uint64_t page_size;
    // Check if mode is SV39
    if(satp.mode != 8) {
        return nullptr;
    }
    // 1-- First we need to get Page Table Entry base addr from SATP register
    uint64_t page_table_addr = ((satp.ppn) << 12);
    FuncReturnFeedback_e feedback = MEMU_OK;
    // 2-- Read Page Table Entry
    // SV39 mechanism is a 3-level page table, we need to read PTE for 3 times to find PAddr
    // SV39 mechanism also supports huge pages, which means the pte in level-2 page table or the level-1 page table
    // could also be the LEAF pte
    // --- Read L2 Page Table (Dir)
    uint16_t l2_vpn = vaddr.vpn_2;
    SV39_PageTableEntry_t l2_pte;
    feedback = sysBus->PAddr_ReadBuffer_MMIOBus_API(
            page_table_addr + (l2_vpn * sizeof(SV39_PageTableEntry_t)),   // Calc PAddr of PTE
            sizeof(SV39_PageTableEntry_t),
            (uint8_t*)&l2_pte);
    assert(feedback == MEMU_OK);
    if(isValidLeafPTE(l2_pte)) {
        page_size = 1 << 30;                                // 1G Huge Page
        return writeTLB(l2_pte, page_size, vaddr);   // Already leaf pte, stop here
    }
    if(!isValidDirPTE(l2_pte)) return nullptr;          // TODO: Replace with handler
    // Calculate address of next level page table
    page_table_addr = (((uint64_t)(l2_pte.PPN2) << 18) | ((uint64_t)(l2_pte.PPN1) << 9) | ((uint64_t)(l2_pte.PPN0))) << 12;
    assert(page_table_addr == ((((((uint64_t)l2_pte.PPN2 << 9) | (uint64_t)l2_pte.PPN1) << 9) | (uint64_t)l2_pte.PPN0) << 12));
    // --- Read L1 Page Table (Dir)
    uint16_t l1_vpn = vaddr.vpn_1;
    SV39_PageTableEntry_t l1_pte;
    feedback = sysBus->PAddr_ReadBuffer_MMIOBus_API(
            page_table_addr + (l1_vpn * sizeof(SV39_PageTableEntry_t)),   // Calc PAddr of PTE
            sizeof(SV39_PageTableEntry_t),
            (uint8_t*)&l1_pte);
    assert(feedback == MEMU_OK);
    if(isValidLeafPTE(l1_pte)) {
        page_size = 1 << 21;                                // 2M Huge Page
        return writeTLB(l1_pte, page_size, vaddr);   // Already leaf pte, stop here
    }
    if(!isValidDirPTE(l1_pte)) return nullptr;
    // Calculate address of next level page table
    page_table_addr = (((uint64_t)(l1_pte.PPN2) << 18) | ((uint64_t)(l1_pte.PPN1) << 9) | ((uint64_t)(l1_pte.PPN0))) << 12;
    assert(page_table_addr == ((((((uint64_t)l1_pte.PPN2 << 9) | (uint64_t)l1_pte.PPN1) << 9) | (uint64_t)l1_pte.PPN0) << 12));
    // --- Read L0 Page Table (Leaf)
    uint16_t l0_vpn = vaddr.vpn_0;
    SV39_PageTableEntry_t l0_pte;
    feedback = sysBus->PAddr_ReadBuffer_MMIOBus_API(
            page_table_addr + (l0_vpn * sizeof(SV39_PageTableEntry_t)),   // Calc PAddr of PTE
            sizeof(SV39_PageTableEntry_t),
            (uint8_t*)&l0_pte);
    assert(feedback == MEMU_OK);
    if(isValidLeafPTE(l0_pte)) {
        page_size = 1 << 12;                                // 4k Usual Page
        return writeTLB(l0_pte, page_size, vaddr);
    }
    else {
        return nullptr;
    }

    assert(0);      // Should never reach here
    return nullptr;
}



