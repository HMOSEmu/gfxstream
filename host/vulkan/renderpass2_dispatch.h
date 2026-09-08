// Copyright 2026 The Android Open Source Project
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <vulkan/vulkan.h>

namespace gfxstream::host::vk {
void logMissingRenderPass2Create();
[[noreturn]] void missingRenderPass2Command(const char* name);

inline VKAPI_ATTR VkResult VKAPI_CALL unavailableCreateRenderPass2(
    VkDevice, const VkRenderPassCreateInfo2*, const VkAllocationCallbacks*, VkRenderPass*) {
    logMissingRenderPass2Create();
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}
inline VKAPI_ATTR void VKAPI_CALL unavailableBeginRenderPass2(
    VkCommandBuffer, const VkRenderPassBeginInfo*, const VkSubpassBeginInfo*) {
    missingRenderPass2Command("vkCmdBeginRenderPass2/core or KHR");
}
inline VKAPI_ATTR void VKAPI_CALL unavailableNextSubpass2(
    VkCommandBuffer, const VkSubpassBeginInfo*, const VkSubpassEndInfo*) {
    missingRenderPass2Command("vkCmdNextSubpass2/core or KHR");
}
inline VKAPI_ATTR void VKAPI_CALL unavailableEndRenderPass2(
    VkCommandBuffer, const VkSubpassEndInfo*) {
    missingRenderPass2Command("vkCmdEndRenderPass2/core or KHR");
}

// VK_KHR_create_renderpass2 was promoted to Vulkan 1.2. An API 1.1 device
// may expose only KHR names; generated decoders and shared handlers use both.
// Only alias pointers already supplied by the driver, preserving distinct
// implementations when both names are available. No feature/version is changed.
template <class Dispatch>
void initRenderPass2Dispatch(Dispatch* vk) {
#define ALIAS_RENDERPASS2(name, unavailable)                 \
    if (!vk->name) vk->name = vk->name##KHR;                  \
    if (!vk->name##KHR) vk->name##KHR = vk->name;              \
    if (!vk->name) {                                         \
        vk->name = unavailable;                             \
        vk->name##KHR = unavailable;                         \
    }
    ALIAS_RENDERPASS2(vkCreateRenderPass2, unavailableCreateRenderPass2)
    ALIAS_RENDERPASS2(vkCmdBeginRenderPass2, unavailableBeginRenderPass2)
    ALIAS_RENDERPASS2(vkCmdNextSubpass2, unavailableNextSubpass2)
    ALIAS_RENDERPASS2(vkCmdEndRenderPass2, unavailableEndRenderPass2)
#undef ALIAS_RENDERPASS2
}
}  // namespace gfxstream::host::vk
