

#include <iostream>

#include "impl/MMIO_Bus_Impl.hpp"
#include "impl/MMIO_MEM_Impl.hpp"


MEmu_MMIOBus sysBus;

int main()
{
    std::cout << "Hello, World!" << std::endl;

    MMIODev_I* sysRAM = new MEmu_MMIO_Mem(1024 * 1024 * 128);

    sysBus.RegisterMMIODev_MMIOBus_API(sysRAM, 0x80000000);


    return 0;
}
