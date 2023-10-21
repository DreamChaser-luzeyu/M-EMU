#include <iostream>

#include "sdk/test/test.h"
#include "misc/obj_factory.h"

TEST_CASE(test_case_1, "CASE 1 !") {
    std::cout << "TEST CASE 1 RUNNING" << std::endl;
}

TEST_CASE(test_case_2, "CASE 2 !") {
    std::cout << "TEST CASE 2 RUNNING" << std::endl;
}

TEST_CASE(test_obj_factory_match, "Object Factory Desc Match Test") {
    bool is_desc_match(const InstanceDesc_t*, const InstanceDesc_t*);
    InstanceDesc_t desc_1 = { .desc_str = "abc" };
    InstanceDesc_t desc_2 = { .desc_str = "abc" };
    InstanceDesc_t desc_3 = { .desc_str = "def" };
    assert(is_desc_match(&desc_1, &desc_2));
    assert(!is_desc_match(&desc_1, &desc_3));
}

using std::string;
using std::vector;
TEST_CASE(test_obj_factory_dev, "Object Factory Get Dev Test") {
    vector<string> get_lib_names(const string& path);
    get_lib_names("./lib/dev");
    MMIODev_I* uart_lite = MMIO_Dev_GetInstance({ .desc_str = "uart_lite" });
    assert(uart_lite);

    MMIODev_I* simple_ram = MMIO_Dev_GetInstance({ .desc_str = "simple_ram" });
    assert(simple_ram);
}

TEST_CASE(test_obj_factory_bus, "Object Factory Get Bus Test") {
    vector<string> get_lib_names(const string& path);
    get_lib_names("./lib/bus");
    MMIOBus_I* bus = Bus_GetInstance({.desc_str = "mmio_bus"});
    assert(bus);
}

TEST_CASE(test_obj_factory_core, "Object Factory Get Core Test") {
    vector<string> get_lib_names(const string& path);
    get_lib_names("./lib/core");
    ProcessorCore_I* core_0 = Core_GetInstance({.desc_str = "rv64core_simple"}, nullptr, 0);
    assert(core_0);
}

TEST_CASE(test_obj_factory_intc, "Object Factory Get Intc Test") {
    vector<string> get_lib_names(const string& path);
    get_lib_names("./lib/intc");
    IntCtrl_I* clint = IntCtrl_GetInstance({.desc_str = "rv_clint"});
    assert(clint);
    IntCtrl_I* plic = IntCtrl_GetInstance({.desc_str = "rv_plic"});
    assert(plic);
}

void test() {
    RunAllTest_Test();
}
