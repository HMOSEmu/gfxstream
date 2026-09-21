#include <vulkan/vulkan.h>
#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "maintenance4_dispatch.h"

struct Dispatch {
#define ENTRY(name) PFN_##name name = nullptr; PFN_##name##KHR name##KHR = nullptr;
    ENTRY(vkGetDeviceBufferMemoryRequirements)
    ENTRY(vkGetDeviceImageMemoryRequirements)
    ENTRY(vkGetDeviceImageSparseMemoryRequirements)
#undef ENTRY
};
static unsigned calls;
static VKAPI_ATTR void VKAPI_CALL buffer(VkDevice, const VkDeviceBufferMemoryRequirements*, VkMemoryRequirements2* out) {
    ++calls; out->memoryRequirements.size = 123;
}
static VKAPI_ATTR void VKAPI_CALL image(VkDevice, const VkDeviceImageMemoryRequirements*, VkMemoryRequirements2* out) {
    ++calls; out->memoryRequirements.size = 456;
}
static VKAPI_ATTR void VKAPI_CALL sparse(VkDevice, const VkDeviceImageMemoryRequirements*, uint32_t* count, VkSparseImageMemoryRequirements2*) {
    ++calls; *count = 7;
}
static VKAPI_ATTR void VKAPI_CALL otherBuffer(VkDevice, const VkDeviceBufferMemoryRequirements*, VkMemoryRequirements2*) {}
static VKAPI_ATTR void VKAPI_CALL otherImage(VkDevice, const VkDeviceImageMemoryRequirements*, VkMemoryRequirements2*) {}
static VKAPI_ATTR void VKAPI_CALL otherSparse(VkDevice, const VkDeviceImageMemoryRequirements*, uint32_t*, VkSparseImageMemoryRequirements2*) {}
int main() {
    for (bool khr : {true, false}) {
        Dispatch d;
        (khr ? d.vkGetDeviceBufferMemoryRequirementsKHR : d.vkGetDeviceBufferMemoryRequirements) = buffer;
        (khr ? d.vkGetDeviceImageMemoryRequirementsKHR : d.vkGetDeviceImageMemoryRequirements) = image;
        (khr ? d.vkGetDeviceImageSparseMemoryRequirementsKHR : d.vkGetDeviceImageSparseMemoryRequirements) = sparse;
        gfxstream::host::vk::initMaintenance4Dispatch(&d);
        assert(d.vkGetDeviceBufferMemoryRequirements && d.vkGetDeviceBufferMemoryRequirementsKHR);
        assert(d.vkGetDeviceImageMemoryRequirements && d.vkGetDeviceImageMemoryRequirementsKHR);
        assert(d.vkGetDeviceImageSparseMemoryRequirements && d.vkGetDeviceImageSparseMemoryRequirementsKHR);
        for (auto f : {d.vkGetDeviceBufferMemoryRequirements, d.vkGetDeviceBufferMemoryRequirementsKHR}) {
            VkMemoryRequirements2 out{}; f(nullptr, nullptr, &out); assert(out.memoryRequirements.size == 123);
        }
        for (auto f : {d.vkGetDeviceImageMemoryRequirements, d.vkGetDeviceImageMemoryRequirementsKHR}) {
            VkMemoryRequirements2 out{}; f(nullptr, nullptr, &out); assert(out.memoryRequirements.size == 456);
        }
        for (auto f : {d.vkGetDeviceImageSparseMemoryRequirements, d.vkGetDeviceImageSparseMemoryRequirementsKHR}) {
            uint32_t count = 0; f(nullptr, nullptr, &count, nullptr); assert(count == 7);
        }
    }
    assert(calls == 12);
    Dispatch both;
    both.vkGetDeviceBufferMemoryRequirements = buffer; both.vkGetDeviceBufferMemoryRequirementsKHR = otherBuffer;
    both.vkGetDeviceImageMemoryRequirements = image; both.vkGetDeviceImageMemoryRequirementsKHR = otherImage;
    both.vkGetDeviceImageSparseMemoryRequirements = sparse; both.vkGetDeviceImageSparseMemoryRequirementsKHR = otherSparse;
    gfxstream::host::vk::initMaintenance4Dispatch(&both);
    assert(both.vkGetDeviceBufferMemoryRequirements == buffer && both.vkGetDeviceBufferMemoryRequirementsKHR == otherBuffer);
    assert(both.vkGetDeviceImageMemoryRequirements == image && both.vkGetDeviceImageMemoryRequirementsKHR == otherImage);
    assert(both.vkGetDeviceImageSparseMemoryRequirements == sparse && both.vkGetDeviceImageSparseMemoryRequirementsKHR == otherSparse);
    Dispatch absent;
    gfxstream::host::vk::initMaintenance4Dispatch(&absent);
    assert(!absent.vkGetDeviceBufferMemoryRequirements && !absent.vkGetDeviceBufferMemoryRequirementsKHR);
    assert(!absent.vkGetDeviceImageMemoryRequirements && !absent.vkGetDeviceImageMemoryRequirementsKHR);
    assert(!absent.vkGetDeviceImageSparseMemoryRequirements && !absent.vkGetDeviceImageSparseMemoryRequirementsKHR);
    puts("PASS: maintenance4 core/KHR dispatch, output forwarding, distinct pointers, and absent support");
}
