#include <vulkan/vulkan.h>
#include <cassert>
#include <cstring>
#include <cstdio>
#ifdef TEST_FIXED
#include "host_query_reset_dispatch.h"
#endif
struct Dispatch { PFN_vkResetQueryPool vkResetQueryPool = nullptr; };
static unsigned calls, lookups;
static VKAPI_ATTR void VKAPI_CALL reset(VkDevice, VkQueryPool, uint32_t first, uint32_t count) {
    assert(first == 3 && count == 7); ++calls;
}
static VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL lookup(VkDevice, const char* name) {
    ++lookups; assert(!strcmp(name, "vkResetQueryPoolEXT"));
    return reinterpret_cast<PFN_vkVoidFunction>(reset);
}
static VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL absent(VkDevice, const char*) { return nullptr; }
static void init(Dispatch* d, bool enabled) {
#ifdef TEST_FIXED
    gfxstream::host::vk::initHostQueryResetDispatch(d, nullptr, lookup, enabled);
#else
    (void)d; (void)enabled; (void)lookup; // Existing decoder has no EXT fallback.
#endif
}
int main() {
    Dispatch disabled; init(&disabled, false); assert(!disabled.vkResetQueryPool && !lookups);
    Dispatch core; core.vkResetQueryPool = reset; init(&core, true);
    assert(core.vkResetQueryPool == reset && !lookups);
    Dispatch ext; init(&ext, true); assert(ext.vkResetQueryPool);
    ext.vkResetQueryPool(nullptr, VK_NULL_HANDLE, 3, 7);
    assert(calls == 1 && lookups == 1);
#ifdef TEST_FIXED
    Dispatch missing;
    gfxstream::host::vk::initHostQueryResetDispatch(&missing, nullptr, absent, true);
    assert(!missing.vkResetQueryPool);
    gfxstream::host::vk::initHostQueryResetDispatch(&missing, nullptr, nullptr, true);
    assert(!missing.vkResetQueryPool);
#endif
    puts("PASS: extension gating, core preservation and query arguments forwarded");
}
