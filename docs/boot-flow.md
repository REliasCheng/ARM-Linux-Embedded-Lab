# Boot Flow

## Reviewed Path

```text
Boot ROM
   ↓
Rockchip Loader / SPL
   ↓
U-Boot
   ↓
Linux Kernel + Board DTB
   ↓
Root Filesystem / Init
   ↓
Userspace
```

## Stage Evidence

| Stage | Evidence in Source Material | Current Status |
| --- | --- | --- |
| Boot ROM | Concept and Rockchip startup notes | Reference only |
| SPL / Loader | Rockchip loader、`idbloader` and image assembly workflow | Reference only |
| U-Boot | Build、ITB and flashing workflow notes | Source/binary not published |
| Kernel | ARM64 build notes and excluded 4.19.232 module metadata | Kernel tree not published |
| DTB | RK3566 board DTS filename and DTC workflow | DTS/DTB not published |
| RootFS | Ubuntu Base ARM64 and Buildroot workflow references | Image/configuration not published |
| Init / Application | Rootfs setup、Qt and device-node access notes | Runtime evidence not provided |

## Interpretation

Boot-chain documentation is not boot evidence. A reproducible result would require exact bootloader and kernel revisions, board configuration, DTB, rootfs input, image assembly steps and a captured serial-console log. Those inputs are not part of this Public Candidate.
