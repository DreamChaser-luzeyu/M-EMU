#pragma once

#include <unordered_map>

#include "sdk/symbol_attr.h"

#include "interface/MMU.h"
#include "rv64_structs.h"
#include "rv64_sv39_structs.h"
#include "rv64_csr_structs.h"
#include "rv64_inst.h"

enum VAddr_RW_Feedback : uint16_t {
    VADDR_ACCESS_OK = 0,
    VADDR_ACCESS_FAULT = 1,             // Error reading from system mmio bus
    VADDR_PAGE_FAULT = 2,

    INST_ACCESS_OK = 0,
    INST_ACCESS_FAULT = 1,              // Error reading from system mmio bus
    INST_PAGE_FAULT = 2,

    VADDR_MIS_ALIGN = 3,
};

static_assert(0b1111111111111111111111111111111111000000000000000000000000000000 == -(1ll << 30));
static_assert(0b1111111111111111111111111111111111111111111000000000000000000000 == -(1ll << 21));
static_assert(0b1111111111111111111111111111111111111111111111111111000000000000 == -(1ll << 12));

class RV64SV39_MMU : public MMU_I {
    // ----- Constants
    const static uint64_t _1G_PAGE_ADDR_MASK = -(1ll << 30);
    const static uint64_t _2M_PAGE_ADDR_MASK = -(1ll << 21);
    const static uint64_t _4k_PAGE_ADDR_MASK = -(1ll << 12);
    const static uint32_t TLB_SIZE = 32;
    // ----- Fields
private:
    PrivMode_e& currentPrivMode;
    SV39_TLB_Entry_t tlb[TLB_SIZE];
//    std::unordered_map<uint64_t, SV39_TLB_Entry_t*> tlbGetCache;  // VPA to TLB_Entry
    uint16_t lastSwappedIndex;
    SATP_Reg_t& satp;
    uint64_t& status;
    uint64_t& rvHartID;
    // --- Used by AMO
    uint64_t lr_pa;
    uint64_t lr_size;
    uint64_t lr_hart;
    bool lr_valid = false;

    // ----- Interface implementation
public:
    ALWAYS_INLINE inline VAddr_RW_Feedback_e VAddr_ReadBuffer_MMU_API(uint64_t begin_addr,
                                                 uint64_t size,
                                                 uint8_t *buffer) override;

    ALWAYS_INLINE inline VAddr_RW_Feedback_e VAddr_WriteBuffer_MMU_API(uint64_t begin_addr,
                                                  uint64_t size,
                                                  const uint8_t *buffer) override;

    ALWAYS_INLINE inline VAddr_RW_Feedback_e VAddr_InstFetch_MMU_API(uint64_t begin_addr, uint64_t size, void* buffer) override;



    // ----- Constructor
public:
    RV64SV39_MMU(MMIOBus_I* bus,
                 PrivMode_e& current_priv_mode,
                 SATP_Reg_t& satp_reg,
                 uint64_t& _status,
                 uint64_t& _hartID) : MMU_I(bus),
                                      lastSwappedIndex(0),
                                      currentPrivMode(current_priv_mode),
                                      satp(satp_reg),
                                      status(_status),
                                      rvHartID(_hartID) {
        for(int i=0; i<TLB_SIZE; i++) tlb[i].pagesize = 0;
    }

    // ----- Member functions
public:
    ALWAYS_INLINE inline SV39_TLB_Entry_t* writeTLB(const SV39_PageTableEntry_t& leaf_pte, uint64_t page_size, const SV39_VAddr_t& vaddr);
    ALWAYS_INLINE inline SV39_TLB_Entry_t* getTLBEntry(SV39_VAddr_t vaddr);
    ALWAYS_INLINE inline RV64_ExecFeedbackCode_e vaddrAtomicMemOperation(uint64_t start_addr, uint64_t size, AMO_Funct_enum op, int64_t src, int64_t& dst);
    ALWAYS_INLINE inline RV64_ExecFeedbackCode_e vaddrLoadReserved(uint64_t start_addr, uint64_t size, uint8_t *buffer);
    ALWAYS_INLINE inline RV64_ExecFeedbackCode_e vaddrStoreConditional(uint64_t start_addr, uint64_t size, const uint8_t *buffer, bool &sc_fail);

    ALWAYS_INLINE inline void SV39_FlushTLB_sfence_vma(uint64_t vaddr, uint64_t asid);
private:
    ALWAYS_INLINE inline bool isValidDirPTE(const SV39_PageTableEntry_t& pte);
    ALWAYS_INLINE inline bool isValidLeafPTE(const SV39_PageTableEntry_t& pte);
    ALWAYS_INLINE inline uint16_t getTLBIndexToSwapOut();
    ALWAYS_INLINE inline bool pa_amo_op(uint64_t pa, uint64_t size, AMO_Funct_enum op, int64_t src, int64_t &dst);
};

// We have to include those implements here because those functions are marked `inline`
#include "rv64_amo.hpp"
#include "rv64_sv39.hpp"



