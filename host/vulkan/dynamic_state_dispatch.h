// Copyright 2026 The Android Open Source Project
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <vulkan/vulkan.h>
namespace gfxstream::host::vk {
[[noreturn]] void missingDynamicStateCommand(const char* name);

inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetCullMode(VkCommandBuffer, VkCullModeFlags) {
    missingDynamicStateCommand("vkCmdSetCullMode/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetFrontFace(VkCommandBuffer, VkFrontFace) {
    missingDynamicStateCommand("vkCmdSetFrontFace/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetPrimitiveTopology(VkCommandBuffer, VkPrimitiveTopology) {
    missingDynamicStateCommand("vkCmdSetPrimitiveTopology/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetViewportWithCount(VkCommandBuffer, uint32_t, const VkViewport*) {
    missingDynamicStateCommand("vkCmdSetViewportWithCount/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetScissorWithCount(VkCommandBuffer, uint32_t, const VkRect2D*) {
    missingDynamicStateCommand("vkCmdSetScissorWithCount/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdBindVertexBuffers2(VkCommandBuffer, uint32_t, uint32_t, const VkBuffer*, const VkDeviceSize*, const VkDeviceSize*, const VkDeviceSize*) {
    missingDynamicStateCommand("vkCmdBindVertexBuffers2/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetDepthTestEnable(VkCommandBuffer, VkBool32) {
    missingDynamicStateCommand("vkCmdSetDepthTestEnable/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetDepthWriteEnable(VkCommandBuffer, VkBool32) {
    missingDynamicStateCommand("vkCmdSetDepthWriteEnable/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetDepthCompareOp(VkCommandBuffer, VkCompareOp) {
    missingDynamicStateCommand("vkCmdSetDepthCompareOp/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetDepthBoundsTestEnable(VkCommandBuffer, VkBool32) {
    missingDynamicStateCommand("vkCmdSetDepthBoundsTestEnable/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetStencilTestEnable(VkCommandBuffer, VkBool32) {
    missingDynamicStateCommand("vkCmdSetStencilTestEnable/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetStencilOp(VkCommandBuffer, VkStencilFaceFlags, VkStencilOp, VkStencilOp, VkStencilOp, VkCompareOp) {
    missingDynamicStateCommand("vkCmdSetStencilOp/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetRasterizerDiscardEnable(VkCommandBuffer, VkBool32) {
    missingDynamicStateCommand("vkCmdSetRasterizerDiscardEnable/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetDepthBiasEnable(VkCommandBuffer, VkBool32) {
    missingDynamicStateCommand("vkCmdSetDepthBiasEnable/EXT");
}
inline VKAPI_ATTR void VKAPI_CALL unavailablevkCmdSetPrimitiveRestartEnable(VkCommandBuffer, VkBool32) {
    missingDynamicStateCommand("vkCmdSetPrimitiveRestartEnable/EXT");
}

// Only promoted commands with identical EXT/core signatures are listed here.
// Copy driver-provided aliases only for extensions enabled on this device;
// Vulkan 1.3 core pointers already present are preserved independently.
template<class Dispatch>
void initDynamicStateDispatch(Dispatch* vk, bool extendedDynamicState,
                              bool extendedDynamicState2) {
#define ALIAS_DYNAMIC_STATE(n, enabled)                   \
    if (enabled) {                                       \
        if (!vk->n) vk->n = vk->n##EXT;                   \
        if (!vk->n##EXT) vk->n##EXT = vk->n;               \
    }                                                    \
    if (!vk->n) vk->n = unavailable##n;                   \
    if (!vk->n##EXT) vk->n##EXT = unavailable##n;
    ALIAS_DYNAMIC_STATE(vkCmdSetCullMode, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetFrontFace, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetPrimitiveTopology, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetViewportWithCount, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetScissorWithCount, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdBindVertexBuffers2, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetDepthTestEnable, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetDepthWriteEnable, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetDepthCompareOp, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetDepthBoundsTestEnable, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetStencilTestEnable, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetStencilOp, extendedDynamicState)
    ALIAS_DYNAMIC_STATE(vkCmdSetRasterizerDiscardEnable, extendedDynamicState2)
    ALIAS_DYNAMIC_STATE(vkCmdSetDepthBiasEnable, extendedDynamicState2)
    ALIAS_DYNAMIC_STATE(vkCmdSetPrimitiveRestartEnable, extendedDynamicState2)
#undef ALIAS_DYNAMIC_STATE
}
}  // namespace gfxstream::host::vk
