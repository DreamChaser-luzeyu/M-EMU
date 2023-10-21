#pragma once

#include "sdk/instance_desc.h"
#include "interface/Core.h"
#include "interface/IntCtrl.h"
#include "interface/MMIO_Dev.h"
#include "interface/MMU.h"
#include "interface/Platform.h"

MMIOBus_I* Bus_GetInstance(const InstanceDesc_t& desc);

ProcessorCore_I *Core_GetInstance(const InstanceDesc_t &desc, MMIOBus_I *bus, uint64_t hart_id);

IntCtrl_I* IntCtrl_GetInstance(const InstanceDesc_t& desc, uint64_t hart_id = 0);

MMIODev_I* MMIO_Dev_GetInstance(const InstanceDesc_t& desc, uint64_t hart_id = 0);

Platform_I* Platform_GetInstance(const InstanceDesc_t& desc);

