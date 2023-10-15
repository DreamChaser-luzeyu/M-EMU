#include <algorithm>
#include "custom/core/RV64SV39_MMU.h"

RV64_ExecFeedbackCode_e RV64SV39_MMU::vaddrAtomicMemOperation(uint64_t start_addr,
                                                              uint64_t size,
                                                              AMO_Funct_enum op,
                                                              int64_t src,
                                                              int64_t &dst) {
    assert(size == 4 || size == 8);
    const SATP_Reg_t* satp_reg = (SATP_Reg_t*)&satp;
    const CSReg_MStatus_t* mstatus = (CSReg_MStatus_t*)&status;

    if ( (currentPrivMode == M_MODE && (!mstatus->mprv || mstatus->mpp == M_MODE))
         || (satp_reg->mode == 0) ) {
        bool amo_fb = pa_amo_op(start_addr, size, op, src, dst);
        if (!amo_fb) { return exec_store_acc_fault; }
        return exec_ok;
    }

    // ----- Need page-based vaddr converting
    // --- Check if data within page region
    if ((start_addr >> 12) != ((start_addr + size - 1) >> 12)) {
        // Out of 4k page range
        return exec_store_misalign;
    }

    SV39_TLB_Entry_t* tlb_entry = getTLBEntry({.val = start_addr});
    if (!tlb_entry || !tlb_entry->A || !tlb_entry->D || !tlb_entry->W) {
        return exec_store_pgfault;
    }
    PrivMode_e priv = (mstatus->mprv && currentPrivMode == M_MODE) ? (PrivMode_e)(mstatus->mpp) : currentPrivMode;
    if (priv == U_MODE && !tlb_entry->U) {
        return exec_store_pgfault;
    }
    if (!mstatus->sum && priv == S_MODE && tlb_entry->U) {
        return exec_store_pgfault;
    }

    uint64_t page_size = (tlb_entry->pagesize == 1) ? (1 << 12) :
                         (tlb_entry->pagesize == 2) ? (1 << 21) :
                         (tlb_entry->pagesize == 3) ? (1 << 30) : 0;
    assert(page_size);
    uint64_t paddr = tlb_entry->ppa + (start_addr % page_size);

    bool amo_fb = pa_amo_op(paddr, size, op, src, dst);
    if (!amo_fb) { return exec_store_acc_fault; }
    return exec_ok;
}

RV64_ExecFeedbackCode_e RV64SV39_MMU::vaddrLoadReserved(uint64_t begin_addr, uint64_t size, uint8_t *buffer) {
    const SATP_Reg_t* satp_reg = (SATP_Reg_t*)&satp;
    const CSReg_MStatus_t* mstatus = (CSReg_MStatus_t*)&status;
    // --- No need to do page-based vaddr converting
    if( (currentPrivMode == M_MODE && (!mstatus->mprv || mstatus->mpp == M_MODE))
        || (satp_reg->mode == 0) ) {
        lr_pa = begin_addr;
        lr_size = size;
        lr_valid = true;
        lr_hart = rvHartID;
        FuncReturnFeedback_e pread_fb = sysBus->PAddr_ReadBuffer_MMIOBus_API(begin_addr, size, buffer);
        if(pread_fb != MEMU_OK) { return exec_store_acc_fault; }
        return exec_ok;
    }
    // ----- Need page-based vaddr converting
    // --- Check if data within page region
    if ((begin_addr >> 12) != ((begin_addr + size - 1) >> 12)) {
        // Out of 4k page range
        return exec_store_misalign;
    }
    SV39_TLB_Entry_t* tlb_entry = getTLBEntry({.val = begin_addr});
    if (!tlb_entry || !tlb_entry->A || (!tlb_entry->R && !(mstatus->mxr && !tlb_entry->X))) {
        return exec_store_pgfault;
    }
    PrivMode_e priv = (mstatus->mprv && currentPrivMode == M_MODE) ? (PrivMode_e)(mstatus->mpp) : currentPrivMode;
    if (priv == U_MODE && !tlb_entry->U) { return exec_store_pgfault; }
    if (!mstatus->sum && currentPrivMode == S_MODE && tlb_entry->U) {
        return exec_store_acc_fault;
    }

    uint64_t page_size = (tlb_entry->pagesize == 1) ? (1 << 12) :
                         (tlb_entry->pagesize == 2) ? (1 << 21) :
                         (tlb_entry->pagesize == 3) ? (1 << 30) : 0;
    assert(page_size);
    uint64_t paddr = tlb_entry->ppa + (begin_addr % page_size);
    lr_pa = begin_addr;
    lr_size = size;
    lr_valid = true;
    lr_hart = rvHartID;
    FuncReturnFeedback_e pread_fb = sysBus->PAddr_ReadBuffer_MMIOBus_API(paddr, size, buffer);
    if (pread_fb != MEMU_OK) { return exec_store_acc_fault; }
    return exec_ok;
}

RV64_ExecFeedbackCode_e RV64SV39_MMU::vaddrStoreConditional(uint64_t begin_addr,
                                                            uint64_t size,
                                                            const uint8_t *buffer,
                                                            bool &sc_fail) {

    const SATP_Reg_t* satp_reg = (SATP_Reg_t*)&satp;
    const CSReg_MStatus_t* mstatus = (CSReg_MStatus_t*)&status;
    // --- No need to do page-based vaddr converting
    if ( (currentPrivMode == M_MODE && (!mstatus->mprv || mstatus->mpp == M_MODE))
         || (satp_reg->mode == 0) ) {
        if (!lr_valid || lr_pa != begin_addr || lr_size != size || lr_hart != rvHartID) {
            // Reserved memory modified, do not preform store
            sc_fail = true;
            if (rvHartID == lr_hart) lr_valid = false;
            return exec_ok;
        }
        sc_fail = false;
        lr_valid = false;
        FuncReturnFeedback_e pwrite_fb = sysBus->PAddr_WriteBuffer_MMIOBus_API(begin_addr,size,buffer);
        if (pwrite_fb != MEMU_OK) { return exec_store_acc_fault; }
        return exec_ok;
    }
    // ----- Need page-based vaddr converting
    // --- Check if data within page region
    if ((begin_addr >> 12) != ((begin_addr + size - 1) >> 12)) {
        // Out of 4k page range
        return exec_store_misalign;
    }
    SV39_TLB_Entry_t* tlb_entry = getTLBEntry({.val = begin_addr});
    if (!tlb_entry || !tlb_entry->A || !tlb_entry->D || !tlb_entry->W) {
        return exec_store_pgfault;
    }
    PrivMode_e priv = (mstatus->mprv && currentPrivMode == M_MODE) ? (PrivMode_e)(mstatus->mpp) : currentPrivMode;
    if (priv == U_MODE && !tlb_entry->U) { return exec_store_pgfault; }
    if (!mstatus->sum && priv == S_MODE && tlb_entry->U) { return exec_store_pgfault; }

    uint64_t page_size = (tlb_entry->pagesize == 1) ? (1 << 12) :
                         (tlb_entry->pagesize == 2) ? (1 << 21) :
                         (tlb_entry->pagesize == 3) ? (1 << 30) : 0;
    assert(page_size);
    uint64_t paddr = tlb_entry->ppa + (begin_addr % page_size);
    if (!lr_valid || lr_pa != begin_addr || lr_size != size || lr_hart != rvHartID) {
        // Reserved memory modified, do not preform store
        sc_fail = true;
        if (rvHartID == lr_hart) lr_valid = false;
        return exec_ok;
    }
    sc_fail = false;
    lr_valid = false;
    FuncReturnFeedback_e pstatus = sysBus->PAddr_WriteBuffer_MMIOBus_API(paddr,size,buffer);
    if (pstatus != MEMU_OK) return exec_store_pgfault;
    return exec_ok;
}

bool RV64SV39_MMU::pa_amo_op(uint64_t pa, uint64_t size, AMO_Funct_enum op, int64_t src, int64_t &dst) {
    int64_t res;
//    bool read_ok = true;
    if (size == 4) {
        int32_t res32;
        /*read_ok &=*/FuncReturnFeedback_e feedback = sysBus->PAddr_ReadBuffer_MMIOBus_API(pa,size,(uint8_t*)&res32);
        if(feedback != MEMU_OK) { return false; }
        res = res32;
    }
    else {
        /*read_ok &=*/FuncReturnFeedback_e feedback = sysBus->PAddr_ReadBuffer_MMIOBus_API(pa,size,(uint8_t*)&res);
        if(feedback != MEMU_OK) { return false; }
    }
//    if (!read_ok) return false;
    int64_t to_write;
    switch (op) {
        case AMOSWAP:
            to_write = src;
            break;
        case AMOADD:
            to_write = src + res;
            break;
        case AMOAND:
            to_write = src & res;
            break;
        case AMOOR:
            to_write = src | res;
            break;
        case AMOXOR:
            to_write = src ^ res;
            break;
        case AMOMAX:
            to_write = std::max(src,res);
            break;
        case AMOMIN:
            to_write = std::min(src,res);
            break;
        case AMOMAXU:
            to_write = std::max((uint64_t)src,(uint64_t)res);
            break;
        case AMOMINU:
            to_write = std::min((uint64_t)src,(uint64_t)res);
            break;
        default:
            assert(false);
    }
    dst = res;

    FuncReturnFeedback_e feedback = sysBus->PAddr_WriteBuffer_MMIOBus_API(pa,size,(uint8_t*)&to_write);
    return feedback == MEMU_OK;
}