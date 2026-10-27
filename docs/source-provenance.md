# Source provenance

| Material | Class | Decision |
| --- | --- | --- |
| Existing repository architecture text and SVG | USER_ORIGINAL | Retained as historical system-path documentation. |
| New C protocol, Linux monitor/adapter, PTY simulator, tests and CI | USER_ORIGINAL | Original implementation; no course source copied. |
| New `kernel/lab_echo.c` | USER_ORIGINAL | Original external module with explicit MIT OR GPL-2.0-only license; built only against distribution headers. |
| Ubuntu 24.04 GCC/Clang, AArch64 cross packages, QEMU and Linux headers | LICENSED_REUSABLE | System dependencies only; exact installed versions emitted by CI; not vendored. |
| Historical Tspi-RK3566 board DTS name, vendor kernel 4.19.232 module metadata, pi/Linux toolchain paths, Ubuntu Base/Buildroot notes | REFERENCE_ONLY / BUILD_EVIDENCE_ONLY | Describe prior material, not this build or a compatible current ABI. |
| Course drivers, vendor BSP/DTS, historical RootFS, toolchain archives and third-party device assets | RIGHTS_UNVERIFIED | Excluded; possession does not establish authorship or public redistribution rights. |

The historical material references `tspi-rk3566-user-v10-linux.dts` and excluded `.ko` metadata reporting `4.19.232` and AArch64. No matching board revision, prepared kernel tree or current device log is available for this public Host foundation. No private credentials, real device path, board operations or external service access are included.

The prior read-only audit recorded inventory SHA-256 `2CFC20CED9849C8EC6DA061FC023C5B6416E2FFA6A6E9F0FE41467BF3D4F663B`. Its recorded original course source path was unavailable during this continuation, so no new claim of direct inspection of that source tree is made.
