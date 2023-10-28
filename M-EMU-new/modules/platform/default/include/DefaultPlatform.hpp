#pragma once

#include <fstream>
#include <filesystem>

#include "interface/Platform.h"
#include "misc/obj_factory.h"



class DefaultPlatform : public Platform_I {
    // ----- Fields
    // --- Bus
    MMIOBus_I* sysBus;
    // --- Cores
    ProcessorCore_I* core0 = nullptr;
    ProcessorCore_I* core1 = nullptr;
    // --- Intcs
    IntCtrl_I* rvPLIC = nullptr;
    IntCtrl_I* rvCLInt = nullptr;
    // --- Devs
    MMIODev_I* uartlite = nullptr;
    MMIODev_I* ram = nullptr;

    // ----- Interface implementation & polymorphic override
public:

    FuncReturnFeedback_e Init_Platform_API() override {
        FuncReturnFeedback_e feedback = MEMU_OK;
        feedback = uartlite->Init_MMIODev_API();
        if(feedback != MEMU_OK) return feedback;
        return MEMU_OK;
    }

    FuncReturnFeedback_e Run_Platform_API() override {
        while(true) {
            // --- Update intc
            rvCLInt->UpdateIntState_IntCtrl_API();
            rvPLIC->UpdateIntState_IntCtrl_API();
            // --- Do exec
            core0->Step_CoreAPI();
            core1->Step_CoreAPI();
            // --- Dev action
            static int counter = 0;
            counter ++;
            if(counter == 100) {
                counter = 0;
                uartlite->Step_MMIODev_API();
            }
        }
        return MEMU_OK;
    }

    FuncReturnFeedback_e WaitForDebugger_Platform_API() override {

        return MEMU_OK;
    }
    // ----- Constructor & Destructor

    DefaultPlatform() {
        // ----- Get used instances from factory
        sysBus = Bus_GetInstance({ .desc_str = "mmio_bus" });
        core0 = Core_GetInstance({ .desc_str = "rv64core_simple" }, sysBus, 0);
        core1 = Core_GetInstance({ .desc_str = "rv64core_simple" }, sysBus, 1);
        rvCLInt = IntCtrl_GetInstance({ .desc_str = "rv_clint" });
        rvPLIC = IntCtrl_GetInstance({ .desc_str = "rv_plic" });
        ram = MMIO_Dev_GetInstance({ .desc_str = "simple_ram" });
        uartlite = MMIO_Dev_GetInstance({ .desc_str = "uart_lite" });
        // ----- Init the platform
        // --- Init cores
        core0->WriteProgramCounter_CoreAPI({ .u64_val = 0x80000000 });
        core1->WriteProgramCounter_CoreAPI({ .u64_val = 0x80000000 });
        core1->setGPRByIndex_CoreAPI(10, 1);
        // --- Init image
        const char* init_file = "/home/luzeyu/temp/memu_linux/opensbi-1.3.1/build/platform/generic/firmware/fw_payload.bin";
        std::ifstream file(init_file,std::ios::in | std::ios::binary);
        uint64_t file_size = std::filesystem::file_size(init_file);
        uint8_t* bin_data = new uint8_t[file_size];
        file.read((char*)bin_data,file_size);
        ram->WriteBuffer_MMIODev_API(0, file_size, bin_data);
        // --- Register devs to bus
        sysBus->RegisterMMIODev_MMIOBus_API(ram, 0x80000000);
        sysBus->RegisterMMIODev_MMIOBus_API(uartlite, 0x60100000);
        // --- Connect peripherals&cores to intcs
        rvCLInt->AttachCoreIntStatus(core0->getIntStatusPtr_Core(), 0);
        rvCLInt->AttachCoreIntStatus(core1->getIntStatusPtr_Core(), 1);
        rvPLIC->AttachCoreIntStatus(core0->getIntStatusPtr_Core(), 0);
        rvPLIC->AttachCoreIntStatus(core1->getIntStatusPtr_Core(), 1);
        rvPLIC->RegisterDev_IntCtrl_API(1, uartlite);
        // --- Register intcs to bus
        sysBus->RegisterMMIODev_MMIOBus_API(rvCLInt, 0x2000000);
        sysBus->RegisterMMIODev_MMIOBus_API(rvPLIC, 0xc000000);
    }
};

