# System Architecture

## Scope

本页描述历史资料能够支持的 RK3566 ARM64 系统分层关系。另有独立的 [Host Linux PTY 运行工程](linux-device-runtime.md)；Host 的设备节点证据不表示该 RK3566 系统已启动。

## Layer Map

| Layer | Observed Evidence | Public Status |
| --- | --- | --- |
| Platform | Tspi-RK3566 board DTS name、Rockchip RK3566 paths、AArch64 module metadata | Documented |
| Boot | Boot ROM、SPL、U-Boot、Rockchip loader and image workflow notes | Reference only |
| Kernel | External module Makefiles and excluded modules reporting kernel 4.19.232 | Metadata reviewed |
| Device Description | `tspi-rk3566-user-v10-linux.dts` reference and DTC exercises | Source not included |
| Driver Interface | Module、character device、GPIO、SPI and block-device examples | Course/unknown-rights source excluded |
| Root Filesystem | Ubuntu Base 18.04.5 ARM64 and Buildroot workflow notes | Images/configuration not included |
| Userspace | Device-node test paths、Qt 5.12.10 cross-build and ADB references | Runtime not reproduced |

## Data and Control Path

```text
Userspace Process
    │  open / read / write
    ▼
Device Node / Kernel Interface
    │  file_operations or bus callback
    ▼
Kernel Driver Model
    │  Device Tree / platform data / bus binding
    ▼
RK3566 Peripheral Boundary
```

该路径只说明资料中出现的接口关系。没有公开 driver source、DTS、kernel tree 或 target log，因此不能据此推断板端运行成功。

## Evidence Boundary

- `Present` 表示源资料中存在可定位文件或 metadata。
- `Reference only` 表示资料说明了流程，但本仓库不包含可执行输入。
- `Not Provided` 表示对应历史 RK3566 层没有可公开复核的 build、hardware 或 runtime evidence；新的 Host 与 AArch64 用户态验证分层见 [Verification](verification.md)。
