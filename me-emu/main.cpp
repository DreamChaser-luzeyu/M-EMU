#include <iostream>
#include "api/core/mmu/paging.h"
#include "struct/misc/status_enum.h"

int main() {
    extern void test_run();
    test_run();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
