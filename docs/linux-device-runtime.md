# Linux PTY device runtime

This project implements a Linux userspace monitor in C11 and an independent Python device simulator. The simulator creates a PTY with `pty.openpty()`, publishes the real `/dev/pts/<N>` slave path, and writes protocol bytes to its master. The monitor opens the slave using `O_RDWR | O_NOCTTY | O_NONBLOCK | O_CLOEXEC`, configures raw `termios`, uses `poll`, `read`, bounded `write`, and closes the descriptor on every exit path. The Linux kernel PTY driver transports the bytes. A PTY is a real kernel device interface, but it is not RK3566 UART, SPI, I2C or a physical board.

## Reproduce on Ubuntu 24.04

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
python3 tests/pty_integration.py --scenario normal --monitor build/device_monitor --simulator tools/pty_device_simulator.py
python3 tests/pty_integration.py --scenario malformed --monitor build/device_monitor --simulator tools/pty_device_simulator.py
python3 tests/pty_integration.py --scenario disconnect --monitor build/device_monitor --simulator tools/pty_device_simulator.py
python3 tests/pty_integration.py --scenario timeout --monitor build/device_monitor --simulator tools/pty_device_simulator.py
```

The four scenarios have finite waits and return nonzero if any required event or exit code is missing. Evidence under `build/artifacts/` includes monitor and simulator logs plus `result.json`. The harness terminates only children it created. CTest has a 12-second outer timeout.

## Framing and model

Frame bytes are `A5 5A | type:u8 | sequence:u16 big-endian | length:u8 | payload:0..16 bytes | xor:u8`. XOR covers all preceding bytes, including magic. Types are DATA=1, COMMAND=2 and ACK=3. The parser has a fixed 23-byte buffer, rejects oversized length/type/checksum, and drops one byte to resynchronize after corruption. DATA has exactly one unsigned value byte. The first DATA sequence is accepted; subsequent DATA must increment modulo 65536. A command is `PING` with sequence 1, and the simulator's acknowledgement is `PONG` with the same sequence. This narrow protocol is an original lab contract, not a vendor device protocol.

Monitor states are INITIALIZED → OPENING → RUNNING → STOPPED on normal completion. Invalid frames pass through PROTOCOL_ERROR and resume RUNNING without changing accepted data. Timeout and PTY master close enter TIMEOUT or DISCONNECTED, respectively, before resource cleanup and STOPPED. The monitor uses a five-second operation deadline (700 ms in the timeout scenario), a 500 ms command write deadline, and rejects unexpected disconnects. No stale `/dev/pts` path is retried: the harness must create a new PTY for a new run. The simulator and monitor each own only their own descriptors.

`normal` sends split DATA seq 1, receives the monitor's actual COMMAND over the PTY, returns ACK, then sends DATA seq 2. `malformed` adds noise, wrong checksum, oversized length and out-of-order DATA seq 3 before valid seq 2. `disconnect` closes the master after DATA seq 1; `timeout` leaves the node open without sending data. `read`, `write`, `poll`, `EINTR`, `EAGAIN`, EOF/HUP and partial-write paths are handled in the Linux adapter and monitor. The real PTY scenarios exercise the ordinary kernel paths; uncommon error branches are not presented as separately injected syscall evidence.
