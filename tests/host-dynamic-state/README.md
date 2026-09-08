# Promoted dynamic-state dispatch regression

Core QEMU PID 31384 (2026-09-08) crashed at PC=0 with x7=0. Its backend was
`host-build/renderpass2-fix/libgfxstream_backend.so`, Build ID
`9ac11e907a1c994325dad7b69e1059fff44ecfa2`, mapped at 0xffffa94f0000.
LR=0xffffa96dc4bc; call instruction offset 0x1ec4b8 resolves via llvm-addr2line
to `vk_sub_decoder.cpp:2502`, calling `vkCmdBindVertexBuffers2`. This is a
Vulkan 1.3 core command with an identical EXT_extended_dynamic_state alias.
The guest instance is Vulkan 1.1. The generated decoder calls the core spelling.

The fix normalizes all 12 commands promoted from EXT_extended_dynamic_state
and the three promoted commands from EXT_extended_dynamic_state2 immediately
after device dispatch initialization. Extension aliases are used only when the
corresponding extension was enabled for the host logical device. Existing core
and EXT pointers remain intact when both exist. Neither feature advertisement
nor API version changes; non-promoted commands are untouched. A missing command
gets an explicit fatal diagnostic, never a silent no-op.

Run `./tests/host-dynamic-state/run.sh`. The test calls both spellings for all
15 pairs in EXT-only and core-only dispatches, preserves distinct pointers,
and checks that missing implementations and disabled extensions fail. The
production fatal handler throws in the unit test so all failures can be checked.
Before normalization, the EXT-only table failed its nonnull core assertion
(exit 134); with the fix it exits 0. Also rerun
`./tests/host-renderpass2/run.sh` to retain coverage of the earlier correction.

Incremental build: `ninja -C host-build -j4 gfxstream_backend`.
A deployment copy is kept in `host-build/dynamic-state-fix/`; the prior
`host-build/renderpass2-fix/` copy remains unchanged for rollback/core symbols.
This standalone test does not prove the complete guest ANGLE render flow.

With both fixes, the guest's draw/finish/readback stages completed and verified
red center / blue corner pixels while the host stayed stable. After the separate
guest cleanup correction, `kernel66-angle-cleanup-01` also completed all cleanup
stages and returned `ANGLE_EXIT=0`. See
[`../HOST_VULKAN_DISPATCH.md`](../HOST_VULKAN_DISPATCH.md) for exact logs, artifact
identities, dependency versions, and the standalone build script.
