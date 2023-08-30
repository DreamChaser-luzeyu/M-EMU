// --- Library include
#include <cstring>
#include <thread>
#include <termios.h>
#include <unistd.h>
// --- API include
#include "api/core/cpu/exec.h"
#include "api/core/mmio_bus/mmio.h"
// --- Project include
#include "memory_bus.hpp"
#include "uartlite.hpp"
#include "ram.hpp"
#include "RVCore.hpp"
#include "rv_systembus.hpp"
#include "rv_clint.hpp"
#include "RVInt.hpp"

bool riscv_test = false;

void uart_input(uartlite& uart)
{
    termios tmp;
    tcgetattr(STDIN_FILENO, &tmp);
    tmp.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &tmp);
    while (1) {
        char c = getchar();
        if (c == 10) c = 13;   // convert lf to cr
        uart.putc(c);
    }
}

RVCore* rv_0_ptr;
RVCore* rv_1_ptr;

void test_run()
{

    // Build opensbi with
    // `make CROSS_COMPILE=riscv64-unknown-linux-gnu- PLATFORM=generic PLATFORM_RISCV_ISA=rv64ima_zicsr_zifencei FW_FDT_PATH=cemu.dtb`

//    int argc;
//    const char* argv[10];
    const char* load_path = "/home/luzeyu/touchfish/ysys-verilator-study/ME-EMU/fw_payload.bin";
//    if (argc >= 2) load_path = argv[1];
//    for (int i = 1; i < argc; i++)
//        if (strcmp(argv[i], "-rvtest") == 0) riscv_test = true;

    rv_systembus system_bus;

    uartlite uart;
    rv_clint<2> clint;
    RVInt<4, 4> plic;
    ram dram(4096l * 1024l * 1024l, load_path);
    assert(system_bus.add_dev(0x2000000, 0x10000, &clint));
    assert(system_bus.add_dev(0xc000000, 0x4000000, &plic));
    assert(system_bus.add_dev(0x60100000, 1024 * 1024, &uart));


    assert(system_bus.add_dev(0x80000000, 2048l * 1024l * 1024l, &dram));

    RVCore rv_0(system_bus, 0);
    rv_0_ptr = &rv_0;
    RVCore rv_1(system_bus, 1);
    rv_1_ptr = &rv_1;

    std::thread uart_input_thread(uart_input, std::ref(uart));

    rv_0.jump(0x80000000);
    rv_1.jump(0x80000000);
    rv_1.set_GPR(10, 1);
    while (1) {
        clint.tick();
        plic.update_ext(1, uart.irq());
        rv_0.step(plic.get_int(0), clint.m_s_irq(0), clint.m_t_irq(0), plic.get_int(1));
//        rv_1.step(plic.get_int(2), clint.m_s_irq(1), clint.m_t_irq(1), plic.get_int(3));
        while (uart.exist_tx()) {
            char c = uart.getc();
            if (c != '\r') { std::cout << c; }
        }
        std::cout.flush();
    }
}