# Driver Model

## Reviewed Interfaces

| Interface | Evidence Observed | Publication Decision |
| --- | --- | --- |
| Kernel Module | init/exit、parameter and printk examples | Source excluded |
| Character Device | `file_operations` and `/dev/*` userspace access | Source excluded |
| User/Kernel Copy | `copy_to_user` / `copy_from_user` examples | Source excluded |
| GPIO | Direct-register and GPIO-subsystem examples | Source and screenshots excluded |
| SPI | SPI OLED driver and userspace device-node example | Source excluded |
| Block Device | RAM-backed block-device examples | Source and modules excluded |

## Why Source Is Not Published

这些文件位于课程日次目录，没有仓库级许可证，并出现多个不同作者标记及生成文件。`MODULE_LICENSE("GPL")` 描述 kernel module 的声明，不足以证明相邻课程源码的作者身份或再分发权。因此本仓库只保留接口层面的原创总结。

## Current Driver Status

```text
HISTORIC_COURSE_DRIVER_SOURCE_PUBLIC=NO
ORIGINAL_MISC_MODULE_SOURCE_PUBLIC=YES
ORIGINAL_MISC_MODULE_LOAD=NOT_PERFORMED
RK3566_DEVICE_BINDING=NOT_PROVIDED
HOST_PTY_USERSPACE_RUNTIME=VERIFIED_PR_CI_40440344040
```

An original, separate [misc-device module](../kernel/lab_echo.c) passed the build-only x86_64 Host Kbuild job in [PR CI run 40440344040](https://github.com/REliasCheng/ARM-Linux-Embedded-Lab/actions/runs/40440344040). The real Host device I/O runtime uses the kernel's PTY driver; it does not exercise the new module or any RK3566 peripheral. The build ABI and no-load boundary are described in [module boundary](kernel-module-boundary.md).
