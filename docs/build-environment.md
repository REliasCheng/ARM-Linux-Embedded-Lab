# Build Environment

## Observed Environment

| Field | Source Evidence | Public Interpretation |
| --- | --- | --- |
| Host | Ubuntu-based workflow references | Exact reproducible host image not provided |
| Architecture | `ARCH=arm64` and AArch64 module metadata | ARM64 target confirmed |
| Cross Compiler | `aarch64-linux-gnu` and Linaro GCC 7.5.0 references | Toolchain archive excluded |
| Kernel Modules | External-module Makefiles targeting a vendor kernel tree | Makefiles/source excluded by rights boundary |
| Kernel Evidence | Excluded `.ko` files report `4.19.232` | Historical build artifact only |
| RootFS | Ubuntu Base 18.04.5 ARM64 and Buildroot references | No rootfs image or overlay published |
| Qt | Qt 5.12.10 ARM64 configuration path | Application build/runtime not reproduced |

## Reproduction Requirements

A future RK3566 target build would need all of the following before target build claims are appropriate:

1. An identified and redistributable board SDK or upstream source baseline.
2. Exact kernel revision and matching headers.
3. Exact AArch64 toolchain version and target triplet.
4. Rights-cleared DTS/configuration changes or patches.
5. Isolated build output plus retained logs.
6. Board or QEMU runtime evidence appropriate to the produced artifacts.

## Current Host and cross-build path

The repository now contains original C11 Host userspace software, a native Linux PTY integration harness, an Ubuntu 24.04 AArch64 cross-build job and QEMU user-mode execution. See [Linux device runtime](linux-device-runtime.md), [AArch64 verification](aarch64-build-verification.md), and the CI workflow for actual build results. The reviewed Linaro/vendor toolchains and RK3566 kernel tree remain excluded; distribution cross-build success does not establish compatibility with the historical board or kernel ABI.
