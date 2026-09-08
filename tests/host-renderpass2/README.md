# Renderpass2 core/KHR dispatch regression

An ANGLE guest using Vulkan 1.1 plus VK_KHR_create_renderpass2 reached
`on_vkCreateRenderPass2`, which called a null core `vkCreateRenderPass2`
function pointer. The host's device dispatch only exposed the KHR name.
KHR Begin also shares the core handler; generated Next/End decoders directly
use either spelling. The fix normalizes all four pairs once, immediately after
loading the device dispatch table, without changing advertised versions,
extensions, features, or generated sources.

`./tests/host-renderpass2/run.sh` uses native C++17 and bundled Vulkan headers.
It tests KHR-only and core-only tables by calling all eight spellings, verifies
that distinct nonnull implementations remain intact, and checks explicit
failure when neither spelling exists. The missing void-command diagnostic
throws in the test so all three paths can be checked; production terminates
with GFXSTREAM_FATAL rather than accepting a command it cannot execute.
Missing Create logs and returns VK_ERROR_EXTENSION_NOT_PRESENT.

Before the fix, the KHR-only case failed the assertion that core Create was
nonnull (exit 134). After the fix the test exits 0 with its PASS line.

Incremental host build:

```sh
ninja -C host-build -j4 gfxstream_backend
```

Preserve the installed backend before deployment; the build itself does not
install into QEMU or a guest. Runtime guest validation is a separate step.

The original PID 24826 core and subsequent successful pixel readback evidence,
exact artifact identities, and reproducible build entry point are recorded in
[`../HOST_VULKAN_DISPATCH.md`](../HOST_VULKAN_DISPATCH.md). The RenderPass2 fix
alone exposed a second dynamic-state alias failure; both fixes are required for
the documented ANGLE draw path.
