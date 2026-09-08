#include <vulkan/vulkan.h>
#include <cassert>
#include <cstdio>
#include <stdexcept>
#include "renderpass2_dispatch.h"
namespace gfxstream::host::vk {
void logMissingRenderPass2Create() {}
[[noreturn]] void missingRenderPass2Command(const char* name) { throw std::runtime_error(name); }
}
struct Dispatch {
 PFN_vkCreateRenderPass2 vkCreateRenderPass2 = nullptr;
 PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR = nullptr;
 PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2 = nullptr;
 PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR = nullptr;
 PFN_vkCmdNextSubpass2 vkCmdNextSubpass2 = nullptr;
 PFN_vkCmdNextSubpass2KHR vkCmdNextSubpass2KHR = nullptr;
 PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2 = nullptr;
 PFN_vkCmdEndRenderPass2KHR vkCmdEndRenderPass2KHR = nullptr;
};
static int calls;
static VKAPI_ATTR VkResult VKAPI_CALL create(VkDevice, const VkRenderPassCreateInfo2*, const VkAllocationCallbacks*, VkRenderPass*) { ++calls; return VK_SUCCESS; }
static VKAPI_ATTR void VKAPI_CALL begin(VkCommandBuffer, const VkRenderPassBeginInfo*, const VkSubpassBeginInfo*) { ++calls; }
static VKAPI_ATTR void VKAPI_CALL next(VkCommandBuffer, const VkSubpassBeginInfo*, const VkSubpassEndInfo*) { ++calls; }
static VKAPI_ATTR void VKAPI_CALL end(VkCommandBuffer, const VkSubpassEndInfo*) { ++calls; }
static VKAPI_ATTR void VKAPI_CALL otherEnd(VkCommandBuffer, const VkSubpassEndInfo*) { ++calls; }
int main() {
 for (bool khr : {true, false}) {
  Dispatch d;
  (khr ? d.vkCreateRenderPass2KHR : d.vkCreateRenderPass2) = create;
  (khr ? d.vkCmdBeginRenderPass2KHR : d.vkCmdBeginRenderPass2) = begin;
  (khr ? d.vkCmdNextSubpass2KHR : d.vkCmdNextSubpass2) = next;
  (khr ? d.vkCmdEndRenderPass2KHR : d.vkCmdEndRenderPass2) = end;
  gfxstream::host::vk::initRenderPass2Dispatch(&d);
  assert(d.vkCreateRenderPass2 && d.vkCreateRenderPass2KHR);
  assert(d.vkCmdBeginRenderPass2 && d.vkCmdBeginRenderPass2KHR);
  assert(d.vkCmdNextSubpass2 && d.vkCmdNextSubpass2KHR);
  assert(d.vkCmdEndRenderPass2 && d.vkCmdEndRenderPass2KHR);
  assert(d.vkCreateRenderPass2(nullptr,nullptr,nullptr,nullptr)==VK_SUCCESS);
  d.vkCmdBeginRenderPass2(nullptr,nullptr,nullptr);
  d.vkCmdNextSubpass2(nullptr,nullptr,nullptr);
  d.vkCmdEndRenderPass2(nullptr,nullptr);
  assert(d.vkCreateRenderPass2KHR(nullptr,nullptr,nullptr,nullptr)==VK_SUCCESS);
  d.vkCmdBeginRenderPass2KHR(nullptr,nullptr,nullptr);
  d.vkCmdNextSubpass2KHR(nullptr,nullptr,nullptr);
  d.vkCmdEndRenderPass2KHR(nullptr,nullptr);
 }
 assert(calls==16);
 Dispatch both;
 both.vkCmdEndRenderPass2=end; both.vkCmdEndRenderPass2KHR=otherEnd;
 gfxstream::host::vk::initRenderPass2Dispatch(&both);
 assert(both.vkCmdEndRenderPass2==end && both.vkCmdEndRenderPass2KHR==otherEnd);
 Dispatch absent;
 gfxstream::host::vk::initRenderPass2Dispatch(&absent);
 assert(absent.vkCreateRenderPass2(nullptr,nullptr,nullptr,nullptr)==VK_ERROR_EXTENSION_NOT_PRESENT);
 int errors=0;
 try { absent.vkCmdBeginRenderPass2(nullptr,nullptr,nullptr); } catch(const std::runtime_error&) { ++errors; }
 try { absent.vkCmdNextSubpass2(nullptr,nullptr,nullptr); } catch(const std::runtime_error&) { ++errors; }
 try { absent.vkCmdEndRenderPass2(nullptr,nullptr); } catch(const std::runtime_error&) { ++errors; }
 assert(errors==3 && calls==16);
 puts("PASS: renderpass2 KHR-only, core-only, distinct pointers, and missing dispatch");
}
