#pragma once

#include <poll.h>
#include <unistd.h>

#include <iostream>
#include <cstdio>
#include <cassert>
#include <thread>

#include "interface/MMIO_Dev.h"
#include "uartlite.hpp"
#include "mmio_dev.hpp"

using std::cout;

const static char* TTY_DEV_ENV_KEY = "MEMU_UARTLITE_TTY";

class CemuUartAdapter : public MMIODev_I {
    // ----- Fields
private:
    uartlite* cemuUartliteDev;
    FILE* char_dev_file = nullptr;
    std::thread* uartInputThread = nullptr;
    uint8_t uartBuffer[32];

    // ----- Interface implementation
public:
    FuncReturnFeedback_e ReadBuffer_MMIODev_API (uint64_t begin_addr, uint64_t size, uint8_t* buffer) override {
        // Check if relative address in range
        assert(begin_addr + size <= this->addrRegionSize);   // TODO: Replace with panic function
        // Do access device
        bool feedback = cemuUartliteDev->do_read(begin_addr, size, buffer);
        if(!feedback) { return MEMU_UNKNOWN; }
        return MEMU_OK;
    }
    // begin_addr should be in range [0, addrRegionSize]
    FuncReturnFeedback_e WriteBuffer_MMIODev_API(uint64_t begin_addr, uint64_t size, const uint8_t* buffer) override {
        // Check if relative address in range
        assert(begin_addr + size <= this->addrRegionSize);   // TODO: Replace with panic function
        // Do access device
        bool feedback = cemuUartliteDev->do_write(begin_addr, size, buffer);
        if(!feedback) { return MEMU_UNKNOWN; }
        return MEMU_OK;
    }

    bool GetIrqStatus() override { return cemuUartliteDev->irq(); }

    FuncReturnFeedback_e Init_MMIODev_API() override {
        // --- Get path to the character device
        char* char_dev_path = getenv(TTY_DEV_ENV_KEY);
        if(!char_dev_path) return MEMU_MODULE_MISSING_ENV;
        // --- Open the device
        char_dev_file = fopen(char_dev_path, "r+");
        if(!char_dev_file) return MEMU_FILE_OPEN_ERR;

        return MEMU_OK;
    }

    FuncReturnFeedback_e Step_MMIODev_API() override {
        while (cemuUartliteDev->exist_tx()) {
            char c = cemuUartliteDev->getc();
            fputc(c, char_dev_file);
        }
        fflush(char_dev_file);

        struct pollfd p_fds[1];
        p_fds[0].fd = fileno(char_dev_file);
        int poll_feedback = poll(p_fds, 1, 0);
        if(poll_feedback > 0) {
            if((p_fds[0].revents & POLLIN) == POLLIN) {
                ssize_t nr_bytes_read = read(p_fds[0].fd, uartBuffer, 32);
                for(int i = 0; i < nr_bytes_read; i++) {
                    // Uncomment to enable echo
                    // fputc(uartBuffer[i], stdout);
                    cemuUartliteDev->putc(uartBuffer[i]);
                }
            }
        }

        return MEMU_OK;
    }

    // ----- Constructor & Destructor
public:
    explicit CemuUartAdapter(uartlite* cemu_dev, uint64_t size): MMIODev_I(0x00, size), cemuUartliteDev(cemu_dev) {}

    ~CemuUartAdapter() {
        delete this->uartInputThread;
        delete this->cemuUartliteDev;
        fclose(char_dev_file);
    }
private:
    CemuUartAdapter() : MMIODev_I(0x00, 0x00) {}

    // ----- Member functions
public:
    mmio_dev* getCEMUDev() { return this->cemuUartliteDev; }
};
