

#include <iostream>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <fstream>
#include <filesystem>

#include "custom/misc/RVCore.hpp"
#include "custom/misc/RVPLICtrl.hpp"
#include "custom/misc/RVCoreLocalInt.hpp"

#include "impl/MMIOBus_Impl.hpp"
#include "impl/MMIOMem_Impl.hpp"
#include "impl/MMIODev_CEmuAdapter.hpp"
#include "impl/Processor_CEmuAdapter.hpp"
#include "impl/MMIOIntCtrl_CEmuAdapter.h"
#include "interface/CoreIntCtrlBridge.h"
#include "interface/IntCtrl.h"

// TODO: Replace branch with `likely`

MEmu_MMIOBus sysBus;

bool riscv_test = false;

//RVCore* rv_0_ptr;
//RVCore* rv_1_ptr;

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
    const char* init_file = "/home/luzeyu/touchfish/opensbi/build/platform/generic/firmware/fw_jump.bin";
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

//    rv_systembus system_bus_adapter(&sysBus);
//    RVCore rv_0(system_bus_adapter, 0);
//    rv_0_ptr = &rv_0;
//    RVCore rv_1(system_bus_adapter, 1);
//    rv_1_ptr = &rv_1;
    // --- Add core and connect system bus to core
    ProcessorCore_I* core_0 = new CEmuCoreAdapter(&sysBus, 0);
    ProcessorCore_I* core_1 = new CEmuCoreAdapter(&sysBus, 1);



//    rv_0.jump(0x80000000);
//    rv_1.jump(0x80000000);
//    rv_1.set_GPR(10, 1);
    // --- Set core initial state
    RegItemVal_t begin_addr;
    begin_addr.u64_val = 0x80000000;
    core_0->WriteProgramCounter_CoreAPI(begin_addr);
    core_1->WriteProgramCounter_CoreAPI(begin_addr);
    ((CEmuCoreAdapter*)core_1)->setGPR(10, 1);

    // --- Connect core with intc
    ((CemuPlicAdapter*)rv_plic)->setCore0IntStatusPtr(core_0->getIntStatusRef());
    ((CemuPlicAdapter*)rv_plic)->setCore1IntStatusPtr(core_1->getIntStatusRef());
    ((CemuClintAdapter*)rv_cli)->setCore0IntStatusPtr(core_0->getIntStatusRef());
    ((CemuClintAdapter*)rv_cli)->setCore1IntStatusPtr(core_1->getIntStatusRef());

    // --- Connect peripheral with intc
    rv_plic->RegisterDev_IntCtrl_API(1, rv_uart);

    // --- Emulate
    while (1) {
//        RVCoreLocalInt<2>* core_local_int_ctrl = ((RVCoreLocalInt<2>*)(((CemuDevAdapter*)rv_cli)->getCEMUDev()));
//        RVPLICtrl<4,4>* platform_level_int_ctrl = ((RVPLICtrl<4,4>*)(((CemuDevAdapter*)(rv_plic))->getCEMUDev()));

        uartlite* uart = (uartlite*)(((CemuDevAdapter*)(rv_uart))->getCEMUDev());

//        core_local_int_ctrl->tick();
        // --- Update intc
        rv_cli->UpdateIntState_IntCtrl_API();
//        platform_level_int_ctrl->update_ext(1, uart->irq());
        rv_plic->UpdateIntState_IntCtrl_API();
//        rv_0.step(platform_level_int_ctrl->get_int(0), core_local_int_ctrl->m_s_irq(0),
//                  core_local_int_ctrl->m_t_irq(0), platform_level_int_ctrl->get_int(1));
//
//        rv_1.step(platform_level_int_ctrl->get_int(2), core_local_int_ctrl->m_s_irq(1),
//                  core_local_int_ctrl->m_t_irq(1), platform_level_int_ctrl->get_int(3));
        // --- Core do exec
        core_0->Step_CoreAPI();
        core_1->Step_CoreAPI();

        while (uart->exist_tx()) {
            char c = uart->getc();
            if (c != '\r') { std::cout << c; }
        }
        std::cout.flush();
    }

    return 0;
}
