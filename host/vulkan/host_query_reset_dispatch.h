// Copyright 2026 The Android Open Source Project
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <vulkan/vulkan.h>

namespace gfxstream::host::vk {
// API 1.1 devices may provide only the EXT spelling. The guest encoder uses
// the promoted command opcode. Resolve the alias only for an enabled extension;
// this does not enable hostQueryReset or change the advertised API version.
template <class Dispatch>
void initHostQueryResetDispatch(Dispatch* dispatch, VkDevice device,
                               PFN_vkGetDeviceProcAddr getProc, bool extensionEnabled) {
    if (!dispatch->vkResetQueryPool && extensionEnabled && getProc) {
        dispatch->vkResetQueryPool = reinterpret_cast<PFN_vkResetQueryPool>(
            getProc(device, "vkResetQueryPoolEXT"));
    }
}
}  // namespace gfxstream::host::vk
