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
DRIVER_SOURCE_PUBLIC=NO
DRIVER_BUILD=NOT_PERFORMED
DEVICE_BINDING=NOT_PROVIDED
USERSPACE_RUNTIME=NOT_PROVIDED
```
