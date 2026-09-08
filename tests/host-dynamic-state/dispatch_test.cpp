#include <vulkan/vulkan.h>
#include <cassert>
#include <cstdio>
#include <stdexcept>
#include "dynamic_state_dispatch.h"
namespace gfxstream::host::vk {
[[noreturn]] void missingDynamicStateCommand(const char* name) { throw std::runtime_error(name); }
}
#define COMMANDS(X) \
 X(vkCmdSetCullMode) \
 X(vkCmdSetFrontFace) \
 X(vkCmdSetPrimitiveTopology) \
 X(vkCmdSetViewportWithCount) \
 X(vkCmdSetScissorWithCount) \
 X(vkCmdBindVertexBuffers2) \
 X(vkCmdSetDepthTestEnable) \
 X(vkCmdSetDepthWriteEnable) \
 X(vkCmdSetDepthCompareOp) \
 X(vkCmdSetDepthBoundsTestEnable) \
 X(vkCmdSetStencilTestEnable) \
 X(vkCmdSetStencilOp) \
 X(vkCmdSetRasterizerDiscardEnable) \
 X(vkCmdSetDepthBiasEnable) \
 X(vkCmdSetPrimitiveRestartEnable)
struct Dispatch {
#define FIELD(n) PFN_##n n=nullptr; PFN_##n##EXT n##EXT=nullptr;
 COMMANDS(FIELD)
#undef FIELD
};
static unsigned calls;
template<class> struct Fake;
template<class... Args> struct Fake<void(VKAPI_PTR*)(Args...)> {
 static VKAPI_ATTR void VKAPI_CALL call(Args...) { ++calls; }
 static VKAPI_ATTR void VKAPI_CALL other(Args...) { ++calls; }
};
template<class... Args> void invoke(void(VKAPI_PTR* fn)(Args...)) { fn(Args{}...); }
int main() {
 for(bool ext : {true,false}) {
  Dispatch d;
#define SET(n) (ext ? d.n##EXT : d.n)=Fake<PFN_##n>::call;
  COMMANDS(SET)
#undef SET
  gfxstream::host::vk::initDynamicStateDispatch(&d,true,true);
#define CHECK(n) assert(d.n && d.n##EXT); invoke(d.n); invoke(d.n##EXT);
  COMMANDS(CHECK)
#undef CHECK
 }
 assert(calls==60);
 Dispatch both;
#define BOTH(n) both.n=Fake<PFN_##n>::call; both.n##EXT=Fake<PFN_##n>::other;
 COMMANDS(BOTH)
#undef BOTH
 gfxstream::host::vk::initDynamicStateDispatch(&both,true,true);
#define PRESERVED(n) assert(both.n==Fake<PFN_##n>::call && both.n##EXT==Fake<PFN_##n>::other);
 COMMANDS(PRESERVED)
#undef PRESERVED
 Dispatch missing;
 gfxstream::host::vk::initDynamicStateDispatch(&missing,true,true);
 unsigned errors=0;
#define MISSING(n) try { invoke(missing.n); } catch(const std::runtime_error&) { ++errors; }
 COMMANDS(MISSING)
#undef MISSING
 assert(errors==15 && calls==60);
 Dispatch disabled;
#define EXT(n) disabled.n##EXT=Fake<PFN_##n>::call;
 COMMANDS(EXT)
#undef EXT
 gfxstream::host::vk::initDynamicStateDispatch(&disabled,false,false);
#define DISABLED(n) try { invoke(disabled.n); } catch(const std::runtime_error&) { ++errors; }
 COMMANDS(DISABLED)
#undef DISABLED
 assert(errors==30 && calls==60);
 puts("PASS: 15 promoted dynamic-state core/EXT pairs; missing and disabled extensions fail");
}
