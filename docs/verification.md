# Verification

## Evidence Matrix

| Level | Status | Evidence |
| --- | --- | --- |
| Source Review | Passed | 773 files、517 directories and 26,594,378 bytes inventoried read-only |
| Rights Review | Passed for Public Candidate | No original course/vendor/source files included |
| Syntax Validation | Passed | Public SVG parsed as XML；Markdown rendered through GitHub API |
| Host Test | Not Applicable | No executable public code or host-side functional test |
| Build Verification | Not Performed | Required cross-toolchain、kernel tree and headers are unavailable locally |
| QEMU Validation | Not Performed | No matching QEMU setup or bootable public image |
| Hardware Validation | Not Provided | No reproducible RK3566 hardware test record generated in this publication |
| Runtime Evidence | Not Provided | No console log、`dmesg`、device output or application trace published |

## Source Evidence

- AArch64 / ARMv8-A is supported by module metadata and build references.
- Excluded `.ko` files report `vermagic=4.19.232 ... aarch64`.
- Board notes reference `tspi-rk3566-user-v10-linux.dts` and Rockchip RK3566 paths.
- Notes reference U-Boot, Ubuntu Base 18.04.5 ARM64, Buildroot and Qt 5.12.10 workflows.

These observations establish learning scope, not current reproducibility.

## Claim Boundary

- Source existence does not prove authorship or redistribution rights.
- Historical `.ko` files indicate a prior build environment but do not prove current build success.
- A command or screenshot in course notes does not provide current hardware validation.
- No autonomous build, boot, driver or userspace-runtime claim is made.
