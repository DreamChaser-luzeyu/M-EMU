#include <iostream>
#include "api/core/mem/read_write.h"


int main() {
    extern void test_run();
    test_run();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
