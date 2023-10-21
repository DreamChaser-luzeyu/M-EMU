

#include <iostream>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <fstream>
#include <filesystem>

//#include "custom/misc/RVCore.hpp.bak"
#include "custom/misc/RVPLICtrl.hpp"
#include "custom/misc/RVCoreLocalInt.hpp"
#include "builtin/core/RV64SV39_MMU.h"

#include "impl/MMIOBus_Impl.hpp"
#include "builtin/dev/MMIOMem_Impl.hpp"
#include "impl/MMIODev_CEmuAdapter.hpp"
//#include "impl/Processor_CEmuAdapter.hpp"
#include "impl/MMIOIntCtrl_CEmuAdapter.h"
//#include "interface/CoreIntCtrlBridge.h"
#include "interface/IntCtrl.h"
#include "builtin/core/RV64Core.h"

// TODO: Optimize branch with `likely`

MEmu_MMIOBus sysBus;

bool riscv_test = false;

void uart_input(uartlite* uart)
{
    termios tmp;
    tcgetattr(STDIN_FILENO, &tmp);
    tmp.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &tmp);
    while (1) {
        char c = getchar();
        if (c == 10) c = 13;   // convert lf to cr
        uart->putc(c);
    }
}

// TODO: Replace with smart pointer

int main() {

    MMIODev_I* sys_ram = new MEmu_MMIO_Mem(1024 * 1024 * 4096l);
    sysBus.RegisterMMIODev_MMIOBus_API(sys_ram, 0x80000000);
    // --- Load binary file to memory
    const char* init_file = "/media/luzeyu/Files/Study/verilator/ME-EMU/firmware/fw_payload.bin";
    std::ifstream file(init_file,std::ios::in | std::ios::binary);
    uint64_t file_size = std::filesystem::file_size(init_file);
    uint8_t* bin_data = new uint8_t[file_size];
    file.read((char*)bin_data,file_size);
    sys_ram->WriteBuffer_MMIODev_API(0, file_size, bin_data);

    // --- Add dev to system bus
    MMIODev_I* rv_uart = new CemuDevAdapter(new uartlite, 1024 * 1024);
    sysBus.RegisterMMIODev_MMIOBus_API(rv_uart, 0x60100000);
    std::thread uart_input_thread(uart_input, (uartlite*)(((CemuDevAdapter*)rv_uart)->getCEMUDev()));
    // --- Add intc to system bus
    IntCtrl_I* rv_plic = new CemuPlicAdapter(new RVPLICtrl<4,4>, 0x4000000);
    sysBus.RegisterMMIODev_MMIOBus_API(rv_plic, 0xc000000);
    IntCtrl_I* rv_cli = new CemuClintAdapter(new RVCoreLocalInt<2>, 0x10000);
    sysBus.RegisterMMIODev_MMIOBus_API(rv_cli, 0x2000000);

    // --- Add core and connect system bus to core
    ProcessorCore_I* core_0 = /*new CEmuCoreAdapter(&sysBus, 0);*/ new RV64Core(&sysBus, 0);
    ProcessorCore_I* core_1 = /*new CEmuCoreAdapter(&sysBus, 1);*/ new RV64Core(&sysBus, 1);

    // --- Set core initial state
    RegItemVal_t begin_addr;
    begin_addr.u64_val = 0x80000000;
    core_0->WriteProgramCounter_CoreAPI(begin_addr);
    core_1->WriteProgramCounter_CoreAPI(begin_addr);
    ((RV64Core *) core_1)->setGPRByIndex_CoreAPI(10, 1);

    // --- Connect core with intc
    // We cannot move clint into core because we need to attach it to the system bus
    ((CemuPlicAdapter*)rv_plic)->setCore0IntStatusPtr(core_0->getIntStatusPtr_Core());
    ((CemuPlicAdapter*)rv_plic)->setCore1IntStatusPtr(core_1->getIntStatusPtr_Core());
    ((CemuClintAdapter*)rv_cli)->setCore0IntStatusPtr(core_0->getIntStatusPtr_Core());
    ((CemuClintAdapter*)rv_cli)->setCore1IntStatusPtr(core_1->getIntStatusPtr_Core());

    // --- Connect peripheral with intc
    rv_plic->RegisterDev_IntCtrl_API(1, rv_uart);

    // --- Emulate
    for(int i=0; i<11647731; i++) {
        // --- Update intc
        rv_cli->UpdateIntState_IntCtrl_API();
        rv_plic->UpdateIntState_IntCtrl_API();

        // --- Core do exec
        core_0->Step_CoreAPI();
        core_1->Step_CoreAPI();

        uartlite* uart = (uartlite*)(((CemuDevAdapter*)(rv_uart))->getCEMUDev());
        while (uart->exist_tx()) {
            char c = uart->getc();
            if (c != '\r') { std::cout << c; }
        }
        std::cout.flush();
    }

    while (1) {
        // --- Update intc
        rv_cli->UpdateIntState_IntCtrl_API();
        rv_plic->UpdateIntState_IntCtrl_API();

        // --- Core do exec
        core_0->Step_CoreAPI();
        core_1->Step_CoreAPI();

        uartlite* uart = (uartlite*)(((CemuDevAdapter*)(rv_uart))->getCEMUDev());
        while (uart->exist_tx()) {
            char c = uart->getc();
            if (c != '\r') { std::cout << c; }
        }
        std::cout.flush();
    }

    return 0;
}
