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

In [PR CI run 40440344040](https://github.com/REliasCheng/ARM-Linux-Embedded-Lab/actions/runs/40440344040), `aarch64-linux-gnu-gcc` reported Ubuntu GCC 13.3.0 and QEMU reported 8.2.2. `file`/`readelf` identified the monitor as AArch64 ELF with `/lib/ld-linux-aarch64.so.1`. QEMU user mode printed `protocol: PASS` and `normal: PASS node=/dev/pts/0 monitor_rc=0 sim_rc=0` for the AArch64 monitor against the Host kernel's PTY. This is an emulated Linux userspace process; it does not boot a kernel, validate RK3566 board support, exercise its UART, or validate a vendor BSP.
