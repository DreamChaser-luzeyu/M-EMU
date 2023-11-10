#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include "sdk/symbol_attr.h"
#include "interface/MMIO_Bus.h"
#include "interface/MMIO_Dev.h"

// TODO: Replace assert with better way of handling error

class MEmu_MMIOBus : public MMIOBus_I {
    // ----- Fields
private:
    std::vector<MMIODev_I*> mmioDevs;
    std::unordered_map<uint64_t, MMIODev_I*> devFindCache;
    // ----- Interface implementation
public:
    ALWAYS_INLINE inline FuncReturnFeedback_e PAddr_ReadBuffer_MMIOBus_API(uint64_t begin_addr, uint64_t size, uint8_t *buffer) override {
        if(!buffer) { return MEMU_NULL_PTR; }

        MMIODevHandle_t dev;
        FuncReturnFeedback_e feedback = findDevAccordingToAddr(begin_addr, &dev);
        if(feedback == MEMU_NOT_FOUND) return MEMU_NOT_ACCESSIBLE;

        assert(dev);
        dev->ReadBuffer_MMIODev_API(begin_addr - dev->getDevBaseAddr(), size, buffer);

        return MEMU_OK;
    }

    ALWAYS_INLINE inline FuncReturnFeedback_e PAddr_WriteBuffer_MMIOBus_API(uint64_t begin_addr, uint64_t size, const uint8_t *buffer) override {
        if(!buffer) { return MEMU_NULL_PTR; }

        MMIODevHandle_t dev;
        FuncReturnFeedback_e feedback = findDevAccordingToAddr(begin_addr, &dev);
        if(feedback == MEMU_NOT_FOUND) return MEMU_NOT_ACCESSIBLE;

        assert(dev);
        dev->WriteBuffer_MMIODev_API(begin_addr - dev->getDevBaseAddr(), size, buffer);

        return MEMU_OK;
    }

    FuncReturnFeedback_e RegisterMMIODev_MMIOBus_API(MMIODev_I* dev, uint64_t base_addr) override {
        // Set dev base address to device
        dev->setDevBaseAddr(base_addr);
        // Check if address region conflicts
        for(auto d : mmioDevs) { if(dev->isAddrRegionConflict(d)) { assert(0); } }
        // Register dev to array `mmioDevs`
        mmioDevs.push_back(dev);
        // Sort list according to dev address
        std::sort(mmioDevs.begin(), mmioDevs.end(),
                  [](const MMIODev_I* dev_1, const MMIODev_I* dev_2)->bool {
            return (dev_1->getDevBaseAddr()) < (dev_2->getDevBaseAddr());
        });
        return MEMU_OK;
    }
    // ----- Constructor & Destructor
public:
    MEmu_MMIOBus() : MMIOBus_I() {}

    // ----- Member functions
private:
    ALWAYS_INLINE inline FuncReturnFeedback_e findDevAccordingToAddr(uint64_t addr, MMIODevHandle_t* dev_ret) {
        // TODO: Let's check RAM first for better performance
        auto dev_it = devFindCache.find(addr);
        if(likely(dev_it != devFindCache.end())) {
            *dev_ret = dev_it->second;
            return MEMU_OK;
        }
        // TODO: Replace with binary search for better performance
        for(auto d : this->mmioDevs) {
            if(d->isAddrInRange(addr)) {
                devFindCache[addr] = d;     // Add current dev to cache
                *dev_ret = d;
                return MEMU_OK;
            }
        }
        *dev_ret = nullptr;
        return MEMU_NOT_FOUND;
    }
};
