# External kernel module boundary

`kernel/lab_echo.c` is an original, bounded Linux misc-device example with read, write and poll callbacks. Write replaces up to 32 bytes. Read consumes available bytes; nonblocking empty read returns `-EAGAIN`, and blocking read waits interruptibly. The node would be named `arm_linux_lab` if loaded in an authorized isolated system. This task does **not** load it, create that node, or claim runtime behavior.

The source carries `SPDX-License-Identifier: (MIT OR GPL-2.0-only)` and the matching `MODULE_LICENSE("Dual MIT/GPL")` declaration. This is an explicit dual-license grant for this original file; the repository's general MIT license does not relicense Linux headers, the kernel or historical course drivers. Kbuild uses official Linux headers from the Ubuntu 24.04 distribution, with `kernel/Makefile` containing `obj-m += lab_echo.o`.

```sh
KDIR=$(find /usr/src -maxdepth 1 -type d -name 'linux-headers-*-generic' | sort -V | tail -n 1)
make -C "$KDIR" M="$PWD/kernel" modules
file kernel/lab_echo.ko
readelf -h kernel/lab_echo.ko
modinfo kernel/lab_echo.ko
sha256sum kernel/lab_echo.ko
```

[PR CI run 40440344040](https://github.com/REliasCheng/ARM-Linux-Embedded-Lab/actions/runs/40440344040) produced a real x86_64 `kernel/lab_echo.ko` with SHA-256 `6f7177b14f26fef2bb8249bcd3c94b83ce6bdc1686f9b025004585198316a9fb`. Its `modinfo` reported `vermagic: 6.8.0-147-generic`; the CI runner was executing `6.17.0-1022-azure`. This confirms only an x86_64 external-module build against the selected packaged header ABI. No `insmod`, `modprobe`, device-node write, ARM64 kernel build or RK3566 hardware test is authorized or executed. A real ARM64 module requires the exact matching prepared RK3566 kernel build tree, configuration, symbol versions and permitted vendor/upstream source before an ABI claim is possible.
