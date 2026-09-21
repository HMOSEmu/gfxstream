// Copyright 2026 The Android Open Source Project
// SPDX-License-Identifier: Apache-2.0
#pragma once

namespace gfxstream::host::vk {

// VK_KHR_maintenance4 was promoted to Vulkan 1.3. Older host instances can
// expose only KHR names even when the guest uses the core commands. Alias
// only entry points provided by the driver, preserving distinct implementations
// and leaving unsupported commands null. This does not enable any feature.
template <class Dispatch>
void initMaintenance4Dispatch(Dispatch* vk) {
#define ALIAS_MAINTENANCE4(name)                 \
    if (!vk->name) vk->name = vk->name##KHR;      \
    if (!vk->name##KHR) vk->name##KHR = vk->name;
    ALIAS_MAINTENANCE4(vkGetDeviceBufferMemoryRequirements)
    ALIAS_MAINTENANCE4(vkGetDeviceImageMemoryRequirements)
    ALIAS_MAINTENANCE4(vkGetDeviceImageSparseMemoryRequirements)
#undef ALIAS_MAINTENANCE4
}

}  // namespace gfxstream::host::vk
