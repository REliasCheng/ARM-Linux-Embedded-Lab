# Verification

## Current engineering evidence

The [PR CI run 40440344040](https://github.com/REliasCheng/ARM-Linux-Embedded-Lab/actions/runs/40440344040) completed five jobs successfully on Ubuntu 24.04. Each GCC, Clang and application ASan/UBSan job passed **5/5** tests: portable protocol, PTY normal, malformed, disconnect and timeout. The PTY harness uses two independent processes and actual `/dev/pts/<N>` kernel nodes. It checks exit codes, DATA seq 1/2, PING/PONG, parser and sequence rejection, HUP/EOF and timeout, and retains per-scenario logs and `result.json` as CI artifacts. Windows local validation covered only the portable protocol test (1/1); no Linux PTY was present locally.

The AArch64 job used distribution `aarch64-linux-gnu-gcc` 13.3.0 and QEMU user mode 8.2.2. `file` and `readelf` identified the monitor and protocol test as AArch64 ELF with `/lib/ld-linux-aarch64.so.1`. QEMU printed `protocol: PASS` and `normal: PASS node=/dev/pts/0 monitor_rc=0 sim_rc=0` for the AArch64 monitor. This is genuine cross build and emulated userspace execution, not an RK3566 boot.

The external-module job invoked real Kbuild and created an x86_64 `kernel/lab_echo.ko`; `file`, `readelf`, `modinfo` and SHA-256 were recorded. Headers were `6.8.0-147-generic`, while the runner's live kernel was `6.17.0-1022-azure`. The module was not loaded and cannot be claimed compatible with the live runner, ARM64 or RK3566. See [module boundary](kernel-module-boundary.md).

## Historical source evidence and rights

The earlier read-only audit counted 773 source-material files, 517 directories and 26,594,378 bytes. It found Tspi-RK3566 board references, a `tspi-rk3566-user-v10-linux.dts` filename, and excluded `.ko` metadata with `vermagic=4.19.232 ... aarch64`. That inventory established historical scope, not ownership, current buildability or board runtime. The recorded course source path was unavailable during this continuation. No course/vendor source, historical module, RootFS, DTS, toolchain or credential was imported. [Source provenance](source-provenance.md) records classifications.

## Claim boundary

```text
LINUX_HOST_DEVICE_IO_VERIFIED=YES (PR CI; delivery also checks the merged main workflow)
AARCH64_USERSPACE_BUILD_VERIFIED=YES (PR CI)
AARCH64_QEMU_USERSPACE_VERIFIED=YES (PR CI)
HOST_KERNEL_MODULE_BUILD_VERIFIED=YES (PR CI; packaged x86_64 headers)
ARM64_KERNEL_MODULE_BUILD_VERIFIED=NO
KERNEL_MODULE_LOADED_VERIFIED=NO
RK3566_BOARD_BOOT_VERIFIED=NO
RK3566_HARDWARE_VERIFIED=NO
RK3566_DEVICE_TREE_VERIFIED=NO
```

The original [boot flow](boot-flow.md), [system architecture](system-architecture.md) and [Device Tree](device-tree.md) pages continue to describe reference paths. Host PTY I/O, ARM64 QEMU userspace, a build-only x86_64 `.ko`, and RK3566 hardware are separate evidence levels.
