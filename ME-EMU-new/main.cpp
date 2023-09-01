

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
#include "impl/MMIODev_CustomUART_Adapter.hpp"

// TODO: Replace branch with `likely`

MEmu_MMIOBus sysBus;

bool riscv_test = false;

RVCore* rv_0_ptr;
RVCore* rv_1_ptr;

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

typedef uint64_t size_t;

int main()
{
//    std::cout << "Hello, World!" << std::endl;

    MMIODev_I* sys_ram = new MEmu_MMIO_Mem(1024 * 1024 * 4096l);
    sysBus.RegisterMMIODev_MMIOBus_API(sys_ram, 0x80000000);

    const char* init_file = "/home/luzeyu/touchfish/ysys-verilator-study/ME-EMU/fw_payload.bin";
    std::ifstream file(init_file,std::ios::in | std::ios::binary);
    uint64_t file_size = std::filesystem::file_size(init_file);
    uint8_t* bin_data = new uint8_t[file_size];
    file.read((char*)bin_data,file_size);
    sys_ram->WriteBuffer_MMIODev_API(0, file_size, bin_data);

    MMIODev_I* rv_uart = new CemuDevAdapter(new uartlite, 1024 * 1024);
    sysBus.RegisterMMIODev_MMIOBus_API(rv_uart, 0x60100000);

    MMIODev_I* rv_plic = new CemuDevAdapter(new RVPLICtrl<4,4>, 0x4000000);
    sysBus.RegisterMMIODev_MMIOBus_API(rv_plic, 0xc000000);

    MMIODev_I* rv_cli = new CemuDevAdapter(new RVCoreLocalInt<2>, 0x10000);
    sysBus.RegisterMMIODev_MMIOBus_API(rv_cli, 0x2000000);

    rv_systembus system_bus_adapter(&sysBus);
    RVCore rv_0(system_bus_adapter, 0);
    rv_0_ptr = &rv_0;
    RVCore rv_1(system_bus_adapter, 1);
    rv_1_ptr = &rv_1;

    std::thread uart_input_thread(uart_input, (uartlite*)(((CemuDevAdapter*)rv_uart)->getCEMUDev()));

    rv_0.jump(0x80000000);
    rv_1.jump(0x80000000);
    rv_1.set_GPR(10, 1);

    while (1) {
        RVCoreLocalInt<2>* clint = ((RVCoreLocalInt<2>*)(((CemuDevAdapter*)rv_cli)->getCEMUDev()));
        RVPLICtrl<4,4>* plic = ((RVPLICtrl<4,4>*)(((CemuDevAdapter*)(rv_plic))->getCEMUDev()));
        uartlite* uart = (uartlite*)(((CemuDevAdapter*)(rv_uart))->getCEMUDev());

        clint->tick();
        plic->update_ext(1, uart->irq());
        rv_0.step(plic->get_int(0), clint->m_s_irq(0), clint->m_t_irq(0), plic->get_int(1));
        rv_1.step(plic->get_int(2), clint->m_s_irq(1), clint->m_t_irq(1), plic->get_int(3));
        while (uart->exist_tx()) {
            char c = uart->getc();
            if (c != '\r') { std::cout << c; }
        }
        std::cout.flush();
    }

    return 0;
}
