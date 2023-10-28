#pragma once

#include <string>
#include <vector>
#include <iostream>
#if __cplusplus >= 201703L
#include <filesystem>
#include <dlfcn.h>
#endif

#include "interface/MMIO_Bus.h"
#include "interface/MMIO_Dev.h"
#include "misc/obj_factory.h"
#include "sdk/console.h"

using std::string;
using std::vector;
using std::filesystem::directory_iterator;
using std::filesystem::directory_entry;

typedef MMIODev_I*       (*GetDevInstance_API_t)      (void);
typedef MMIOBus_I*       (*GetBusInstance_API_t)      (void);
typedef IntCtrl_I*       (*GetIntcInstance_API_t)     (void);
typedef Platform_I*      (*GetPlatformInstance_API_t) (void);
typedef ProcessorCore_I* (*GetCoreInstance_API_t)     (MMIOBus_I* sys_bus, uint64_t hart_id);

const static char* BUILTIN_RV64_CORE = "rv64core_simple";
const static char* BUILTIN_PLIC = "rv_plic";
const static char* BUILTIN_CLINT = "rv_clint";
const static char* BUILTIN_UARTLITE_DEV = "uart_lite";

const static char* INSTANCE_DESC_SYMBOL_NAME = "InstanceDesc_GV";
const static char* GET_INSTANCE_SYMBOL_NAME = "GetInstance_API";

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::filesystem::directory_iterator;
using std::filesystem::directory_entry;

static vector<string> get_lib_names(const string& path) {
    vector<string> lib_names;

    for(const directory_entry& entry : directory_iterator(path)) {
//        std::cout << entry.path() << std::endl;
        lib_names.push_back(entry.path().string());
    }

    return lib_names;
}

static bool is_desc_match(const InstanceDesc_t* desc_1, const InstanceDesc_t* desc_2) {
    // Does not accept null pointer in this function
    assert(desc_1);
    assert(desc_2);
    if(desc_1 == desc_2) return true;
    if(strcmp(desc_1->desc_str, desc_2->desc_str) == 0) return true;
    return false;
}

static void* get_constructor_func(const InstanceDesc_t& desc, const char* path) {
    vector<string> libs = get_lib_names(path);
    for(const string& s : libs) {
        // --- Dynamic load lib
        void* handle = dlopen(s.c_str(), RTLD_LAZY);
        if(!handle) {
            cout << STYLE_TEXT_RED << "[ERROR] File "<< s << " dlopen failed: " << dlerror() << STYLE_RST << endl;
            dlclose(handle);
            continue;
        }
        // --- Get instance desc
        const InstanceDesc_t* lib_desc_ptr = (const InstanceDesc_t*)dlsym(handle, INSTANCE_DESC_SYMBOL_NAME);
        if(!lib_desc_ptr) {
            cout << STYLE_TEXT_YELLOW << "[WARN ] Symbol " << INSTANCE_DESC_SYMBOL_NAME << " not found in lib " << s << STYLE_RST << endl;
            dlclose(handle);
            continue;
        }
        // --- Match instance desc & return instance constructor ptr
        if(is_desc_match(&desc, lib_desc_ptr)) {
            void* get_instance_api = dlsym(handle, GET_INSTANCE_SYMBOL_NAME);
            if(!get_instance_api) {
                cout << STYLE_TEXT_YELLOW << "[WARN ] Symbol " << GET_INSTANCE_SYMBOL_NAME << " not found in lib " << s << STYLE_RST << endl;
                dlclose(handle);
                continue;
            }
            return get_instance_api;
        }
    }
    cout << STYLE_TEXT_YELLOW << "[WARN ] No matching lib with desc \"" << desc.desc_str << "\"" << STYLE_RST << endl;
    return nullptr;
}

static ProcessorCore_I *Core_GetInstance(const InstanceDesc_t &desc, MMIOBus_I* bus, uint64_t hart_id) {
    GetCoreInstance_API_t get_instance_api = (GetCoreInstance_API_t)get_constructor_func(desc, "./lib/core");
    if(get_instance_api) return get_instance_api(bus, hart_id);
    return nullptr;
}

static IntCtrl_I* IntCtrl_GetInstance(const InstanceDesc_t& desc, uint64_t hart_id) {
    GetIntcInstance_API_t get_instance_api = (GetIntcInstance_API_t)get_constructor_func(desc, "./lib/intc");
    if(get_instance_api) return get_instance_api();
    return nullptr;
}

static MMIOBus_I* Bus_GetInstance(const InstanceDesc_t& desc) {
    GetBusInstance_API_t get_instance_api = (GetBusInstance_API_t)get_constructor_func(desc, "./lib/bus");
    if(get_instance_api) return get_instance_api();
    return nullptr;
}

static MMIODev_I* MMIO_Dev_GetInstance(const InstanceDesc_t& desc, uint64_t hart_id) {
    GetDevInstance_API_t get_instance_api = (GetDevInstance_API_t)get_constructor_func(desc, "./lib/dev");
    if(get_instance_api) return get_instance_api();
    return nullptr;
}

static Platform_I* Platform_GetInstance(const InstanceDesc_t& desc) {
    GetPlatformInstance_API_t get_instance_api = (GetPlatformInstance_API_t)get_constructor_func(desc, "./lib/platform");
    if(get_instance_api) return get_instance_api();
    return nullptr;
}
