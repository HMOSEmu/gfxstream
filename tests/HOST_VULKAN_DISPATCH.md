# Host Vulkan promoted-command fixes

The ANGLE guest requests Vulkan 1.1 while using supported extension commands.
The host decoder sometimes calls the promoted core spelling. Vulkan loaders may
return only the enabled extension spelling for that device, so a direct call to
the unpopulated core dispatch entry crashes at address zero.

These changes normalize driver-provided aliases after logical-device dispatch
initialization. No guest API version, feature bit, or extension advertisement
changes, and generated protocol/decoder sources are unchanged.

- `host-renderpass2`: four Vulkan 1.2 / KHR_create_renderpass2 command pairs.
- `host-dynamic-state`: twelve EXT_extended_dynamic_state and three promoted
  EXT_extended_dynamic_state2 command pairs. Each EXT group is used only when
  its extension appears in the actual host device's enabled extension list.

Both implementations retain distinct nonnull pointers when both spellings
exist. Missing RenderPass2 creation returns VK_ERROR_EXTENSION_NOT_PRESENT;
missing void commands terminate with an explicit diagnostic rather than
silently accepting an unimplemented operation. This does not provide guest
fault isolation for invalid/unsupported command streams.

## Build and tests

Base source commit: `681d81edd2ec597b055c2fbe99a742d95545722a` plus this change.
The standalone native Linux AArch64 build was verified with GCC/G++ 10.3.1,
CMake 3.22.0, Ninja 1.8.2, and host glibc. Vulkan/DRM/GLM/X11/XCB/OpenGL headers
are bundled under `third_party`; no guest musl toolchain or LLVM is used.
This configuration disables the optional gtest, ASTC CPU decoder, Perfetto,
benchmark, and ANGLE shader-parser build dependencies. Use a native shell
without guest CC/CXX/CFLAGS/LDFLAGS overrides.

From this repository:

```sh
./tests/build-host-backend.sh
```

The script explicitly configures the tested SYSTEM/standalone Release options,
runs both targeted suites, incrementally builds `gfxstream_backend`, and copies
the result into a newly created `host-build/verified-backend.XXXXXX` directory
with SHA256 and Build ID. It performs no installation or VM operations. `BUILD`
and `JOBS` can select an independent native build directory and parallelism.
Existing deployed artifact directories are not overwritten. Neither the build
tree nor binaries belong in Git.

Each suite can also run independently through its `run.sh`; it needs only the
native C++17 compiler and bundled headers. The original null-core cases failed
before normalization (exit 134). Both suites pass with these changes, including
missing implementations, preservation of distinct pointers, and disabled EXT
groups. The tests substitute an exception for the production fatal diagnostic
so all missing-command paths can be checked in one process.

## Runtime evidence (2026-09-08 workspace)

Paths below are relative to `/home/sun/7.0`, outside this repository.

| Run | Evidence | Result |
| --- | --- | --- |
| QEMU PID 24826 | `kernel/regression/kernel66-rutabaga-radv-01/host-core-maps.txt` | PC=0; backend call at offset 0x1e148c, return address offset 0x1e1490, resolves to `on_vkCreateRenderPass2` calling null core dispatch. |
| QEMU PID 31384, RenderPass2 fix | `kernel/regression/kernel66-angle-renderpass2-01/host-core-maps.txt` | PC=0, x7=0; call offset 0x1ec4b8 (`vk_sub_decoder.cpp:2502`) invokes `vkCmdBindVertexBuffers2`. |
| Both fixes | `kernel/regression/kernel66-angle-dynamic-state-01/angle-full.txt` | ANGLE shaders, draw, finish, and pixel readback complete; host remains stable. Guest cleanup failed in this earlier run. |
| Both fixes plus guest cleanup correction | `kernel/regression/kernel66-angle-cleanup-01/angle-full.txt` | Center `255,0,0,255`, corner `0,0,255,255`; unbind, context/surface destruction, and EGL termination succeed; `ANGLE_EXIT=0`. |

The last run identifies ANGLE over `Virtio-GPU GFXStream (AMD Radeon Pro WX5100
Graphics (RADV POLARIS10))`. Host RADV is Mesa 24.3.4, Vulkan 1.3.296, selected
through an isolated ICD; the guest gfxstream ICD reports Vulkan 1.1.0. Host RADV
build instructions are in the Mesa repository's `ohos_port/host-radv/`.

The earlier both-fixes run exited 139 during guest cleanup. After the separate
guest ICD TLS/destruction correction, `kernel66-angle-cleanup-01` completed
rendering, readback, all cleanup stages, and process exit successfully. This P3
pbuffer lifecycle result does not validate P4 buffer import or comprehensive
conformance.

Preserved artifact identities:

| Artifact under gfxstream `host-build/` | SHA256 | Build ID |
| --- | --- | --- |
| `renderpass2-fix/libgfxstream_backend.so` | `485741be1d09269891a18105ed391d2b35f639be773ca3193656a9b0d7fbbdf7` | `9ac11e907a1c994325dad7b69e1059fff44ecfa2` |
| `dynamic-state-fix/libgfxstream_backend.so` | `ce31fc89e429d71e1853de981c5406b667611eb74a037c14d040bdde26a8fa38` | `c1bad747979d1652013d63f4f1ecee5dcf2f2126` |

For symbolization, use the exact preserved artifact and `llvm-addr2line -f -C
-i -e LIB OFFSET`. GNU addr2line stalled on these large debug artifacts in this
workspace. The system coredumps were truncated; their crash register and mapping
notes still identify the null indirect call sites.
