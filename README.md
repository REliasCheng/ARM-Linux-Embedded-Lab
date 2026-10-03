# ARM-Linux-Embedded-Lab

面向 Tspi-RK3566 / Rockchip RK3566 的 ARM64 Embedded Linux 架构与系统集成实验室，聚焦启动链、Device Tree、Kernel Driver 与 Userspace Interface 的证据化梳理。

**🐧 Embedded Linux System Path**

![ARM Linux system overview](assets/images/architecture/arm-linux-system-overview.svg)

## System Snapshot

| Field | Current Scope |
| --- | --- |
| Architecture | AArch64 / ARMv8-A |
| Board / SoC | Tspi-RK3566 / Rockchip RK3566（来自资料中的 board DTS 与构建路径） |
| Bootloader | Rockchip SPL / U-Boot workflow reviewed；boot artifacts not published |
| Kernel | 4.19.232 evidenced by excluded AArch64 module metadata；separate notes reference 6.12.9 exercises |
| Root Filesystem | Ubuntu Base 18.04.5 ARM64 and Buildroot workflows referenced；images not published |
| Toolchain | `aarch64-linux-gnu` / Linaro GCC references；toolchain archive not published |
| Focus | Boot flow、Device Tree、external module model、device-node userspace path |
| Verification | Source inventory and documentation checks；build、QEMU、hardware and runtime evidence not provided |

> 🧪 **Evidence:** 已完成 773 个源资料文件的只读盘点，并确认 AArch64、RK3566、U-Boot、Kernel 4.19.232 module metadata、Device Tree、rootfs 与 driver interface 线索。公开仓库不包含课程源码、厂商 SDK、构建产物或运行日志，因此不声明系统已构建、启动或完成板端验证。

## 📌 Overview

本仓库把分散的 ARM Linux 配套资料整理为一条可审查的系统路径，用于说明 bootloader、kernel、device description、driver interface 与 userspace 之间的关系。它采用 Documentation / Architecture Lab 形式，不复制课程讲义、未知权利源码、外部图片、SDK、rootfs 或 toolchain。

公开内容仅由本仓库维护者重新编写的技术说明和自绘架构图组成。资料中出现的构建命令、模块文件和设备操作记录仅作为范围判断依据，不被包装为本仓库已经复现的成果。

## 🧰 Platform & Technology

| Area | Evidence-backed Scope |
| --- | --- |
| CPU Architecture | ARM64 / AArch64；module metadata reports `armv8-a` |
| Target Platform | Tspi-RK3566 board path and Rockchip RK3566 references |
| Boot | Boot ROM、SPL、U-Boot and Rockchip image workflow are documented as reference paths |
| Kernel | External module workflow and 4.19.232 module metadata；no kernel tree included |
| Device Description | `tspi-rk3566-user-v10-linux.dts` is referenced；no DTS/DTSI source included |
| Driver Interfaces | Module、character device、GPIO、SPI and block-device examples were reviewed but excluded |
| Userspace | Device-node access and Qt 5.12.10 cross-build paths are referenced but not published as verified applications |
| RootFS | Ubuntu Base and Buildroot workflows are referenced；no filesystem image or overlay included |

## 🔄 Boot Flow

```text
Boot ROM / Rockchip Loader Reference
                  ↓
            SPL / U-Boot
                  ↓
       Linux Kernel + Board DTB
                  ↓
       Root Filesystem / Init Path
                  ↓
 Driver Interface / Userspace Application
```

这条链路来自资料中的 U-Boot、Rockchip loader、kernel、board DTS 与 rootfs 工作流。公开仓库不包含对应 binary、source tree 或 console log，因此各层表示“已定位的集成路径”，不表示本轮已成功启动。详见 [Boot Flow](docs/boot-flow.md)。

## 🏗️ Architecture

架构审查以三条边界为核心：

- **Boot boundary** — 区分 Boot ROM / loader reference、U-Boot workflow 与实际 boot evidence。
- **Kernel boundary** — 区分 kernel/module metadata、Device Tree reference 与可复现 build。
- **Userspace boundary** — 区分 device-node / Qt integration path 与真实 runtime output。

系统分层和证据状态见 [System Architecture](docs/system-architecture.md)。

## 🧩 Device Tree

资料明确引用 Rockchip ARM64 board DTS `tspi-rk3566-user-v10-linux.dts`，并覆盖 DTS/DTSI、label、alias、node path 与 DTC 基础流程。原始 DTS、vendor include tree 和 DTB 不在源目录中，因此本仓库只记录接口关系，不发布或声称修改过 board Device Tree。详见 [Device Tree](docs/device-tree.md)。

## ⚙️ Kernel & Drivers

只读审查定位到 external module、character device、user/kernel copy、GPIO、SPI OLED 与 RAM-backed block-device 示例。它们处于课程或未知权利边界，且包含不同作者标记与生成物，因此全部从公开仓库排除。

这些材料能证明“接触过哪些接口”，不能证明“这些驱动由仓库维护者原创”或“已在当前硬件运行”。接口映射见 [Driver Model](docs/driver-model.md)。

## 🖥️ Userspace

资料中存在通过 `/dev/*` 访问字符设备和 SPI OLED 的测试程序线索，也有 Qt 5.12.10 ARM64 cross-build 与 ADB deployment 记录。由于原始代码权利边界和运行证据不足，本仓库不公开这些程序，也不声明 GUI 或设备控制已经复现。

## ✨ Key Features

| Capability | Implementation Entry | Engineering Focus |
| --- | --- | --- |
| Layered system map | [System Architecture](docs/system-architecture.md) | Boot、kernel、device description and userspace boundaries |
| Boot-chain evidence map | [Boot Flow](docs/boot-flow.md) | Separate referenced workflow from reproducible boot evidence |
| Device Tree boundary | [Device Tree](docs/device-tree.md) | Board DTS identity、node roles and missing-public-source boundary |
| Driver interface map | [Driver Model](docs/driver-model.md) | Module、character device、GPIO、SPI and block-device relationships |
| Environment boundary | [Build Environment](docs/build-environment.md) | Toolchain、kernel tree and target dependencies |
| Verification matrix | [Verification](docs/verification.md) | Source review versus build、QEMU、hardware and runtime evidence |

## 📂 Project Structure

```text
ARM-Linux-Embedded-Lab/
├── README.md
├── LICENSE
├── THIRD_PARTY_NOTICES.md
├── .gitignore
├── docs/
│   ├── system-architecture.md
│   ├── boot-flow.md
│   ├── build-environment.md
│   ├── device-tree.md
│   ├── driver-model.md
│   └── verification.md
└── assets/images/architecture/
    └── arm-linux-system-overview.svg
```

仓库有意不创建空的 `drivers/`、`device-tree/`、`configs/` 或 `patches/` 目录，因为当前没有权利与来源均明确、可安全发布的对应实现。

## 📚 Documentation

- [System Architecture](docs/system-architecture.md)
- [Boot Flow](docs/boot-flow.md)
- [Build Environment](docs/build-environment.md)
- [Device Tree](docs/device-tree.md)
- [Driver Model](docs/driver-model.md)
- [Verification](docs/verification.md)
- [Third-party Boundary](THIRD_PARTY_NOTICES.md)

## 🧪 Verification

### 🔎 Source Review

**Status:** Passed. 已只读盘点 773 个文件与 517 个目录，并核对技术线索、构建产物、许可证缺失、敏感信息和公开边界。

### ✅ Syntax Validation

**Status:** Passed. 公开 SVG 已通过 XML parse；Markdown 已完成 GitHub rendering 与相对链接检查。

### 💻 Host Test

**Status:** Not Applicable. 当前 Public Candidate 不包含可执行源码或 host-side functional test。

### 🔨 Build Verification

**Status:** Not Performed. 当前主机没有资料所需的 AArch64 cross-toolchain、vendor kernel tree 或 matching headers，本轮未安装额外工具链。

### 🧪 QEMU Validation

**Status:** Not Performed. 当前主机没有对应 QEMU 环境，且公开仓库不包含可启动 image。

### 🔌 Hardware Validation

**Status:** Not Provided. 没有在本轮生成可复核的 RK3566 boot、module load、GPIO、SPI 或 display hardware evidence。

### 📊 Runtime Evidence

**Status:** Not Provided. 没有公开 console log、`dmesg`、device output、Qt rendering 或 application trace。

完整解释见 [Verification](docs/verification.md)。

## License Boundary

根目录 `LICENSE` 仅覆盖本仓库维护者新增的 README、原创技术文档与自绘 SVG。课程资料、外部图片、Linux/U-Boot/Buildroot/Qt 源码、vendor SDK、toolchain、rootfs、kernel modules 与其他二进制均未包含，也不受根目录 MIT License 重新许可；详见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。
