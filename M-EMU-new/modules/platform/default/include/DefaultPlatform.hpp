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

    // ----- Interface implementation
public:
    FuncReturnFeedback_e Run_Platform_API() override {

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
        const char* init_file = "/media/luzeyu/Files/Study/verilator/ME-EMU/firmware/fw_payload.bin";
        std::ifstream file(init_file,std::ios::in | std::ios::binary);
        uint64_t file_size = std::filesystem::file_size(init_file);
        uint8_t* bin_data = new uint8_t[file_size];
        file.read((char*)bin_data,file_size);
        ram->WriteBuffer_MMIODev_API(0, file_size, bin_data);
        // --- Register devs to bus
        sysBus->RegisterMMIODev_MMIOBus_API(ram, 0x80000000);
        sysBus->RegisterMMIODev_MMIOBus_API(uartlite, 0x60100000);
        // --- Connect peripherals&cores to intcs
        // TODO: How to connect core to intcs?
        rvPLIC->RegisterDev_IntCtrl_API(1, uartlite);
        // --- Register intcs to bus
        sysBus->RegisterMMIODev_MMIOBus_API(rvCLInt, 0x2000000);
        sysBus->RegisterMMIODev_MMIOBus_API(rvPLIC, 0xc000000);
    }
};

