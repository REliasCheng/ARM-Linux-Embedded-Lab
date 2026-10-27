# AArch64 userspace verification

The CI AArch64 job uses Ubuntu 24.04 distribution packages `gcc-aarch64-linux-gnu`, `binutils-aarch64-linux-gnu`, `libc6-arm64-cross` and `qemu-user`. It prints actual package/compiler/QEMU versions. It does not import a course toolchain or RK3566 sysroot.

```sh
cmake -S . -B build-arm64 -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64-linux-gnu.cmake
cmake --build build-arm64 --parallel
file build-arm64/device_monitor build-arm64/test_protocol
aarch64-linux-gnu-readelf -h -l build-arm64/device_monitor
qemu-aarch64 -L /usr/aarch64-linux-gnu build-arm64/test_protocol
python3 tests/pty_integration.py --scenario normal --monitor build-arm64/device_monitor --simulator tools/pty_device_simulator.py --wrapper 'qemu-aarch64 -L /usr/aarch64-linux-gnu'
```

`file` and `readelf` must show an AArch64 ELF, and the interpreter/needed libraries belong to the distribution ARM64 userspace. QEMU user mode executes the AArch64 protocol test and, when the CI step passes, the AArch64 monitor against the host kernel's PTY. This is an emulated Linux userspace process; it does not boot a kernel, validate RK3566 board support, exercise its UART, or validate a vendor BSP.
