"""Independent device process backed by a real Linux PTY master."""

import argparse
import os
import pty
import select
import sys
import time


def frame(kind, sequence, payload):
    assert len(payload) <= 16
    data = bytes((0xA5, 0x5A, kind, sequence >> 8, sequence & 255, len(payload))) + payload
    checksum = 0
    for byte in data:
        checksum ^= byte
    return data + bytes((checksum,))


def write_all(fd, data):
    while data:
        data = data[os.write(fd, data):]


def wait_command(master):
    deadline = time.monotonic() + 3
    buffer = b""
    while time.monotonic() < deadline:
        ready, _, _ = select.select([master], [], [], 0.1)
        if ready:
            buffer += os.read(master, 64)
            expected = frame(2, 1, b"PING")
            if expected in buffer:
                print("EVENT COMMAND_RECEIVED seq=1 payload=PING", flush=True)
                write_all(master, frame(3, 1, b"PONG"))
                return
    raise RuntimeError(f"command not received, bytes={buffer.hex()}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--scenario", choices=("normal", "malformed", "disconnect", "timeout"), required=True)
    args = parser.parse_args()
    master, slave = pty.openpty()
    try:
        print(f"READY {os.ttyname(slave)}", flush=True)
        if sys.stdin.readline().strip() != "START":
            raise RuntimeError("START not received")
        os.close(slave)
        slave = -1
        if args.scenario == "timeout":
            time.sleep(1.5)
            return 0
        if args.scenario == "malformed":
            write_all(master, b"bad" + frame(1, 1, b"x")[:-1] + b"\x00")
            write_all(master, b"\xa5\x5a\x01\x00\x01\xff")
            print("EVENT MALFORMED_SENT", flush=True)
        first = frame(1, 1, b"\x2a")
        write_all(master, first[:3])
        time.sleep(0.03)
        write_all(master, first[3:])
        print("EVENT DATA_SENT seq=1", flush=True)
        if args.scenario == "disconnect":
            time.sleep(0.2)
            os.close(master)
            master = -1
            print("EVENT MASTER_CLOSED", flush=True)
            return 0
        wait_command(master)
        if args.scenario == "malformed":
            write_all(master, frame(1, 3, b"\x63"))
            print("EVENT OUT_OF_ORDER_SENT seq=3", flush=True)
        write_all(master, frame(1, 2, b"\x2b"))
        print("EVENT DATA_SENT seq=2", flush=True)
        time.sleep(0.5)
        return 0
    finally:
        if slave >= 0:
            os.close(slave)
        if master >= 0:
            os.close(master)


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception as error:
        print(f"SIMULATOR_FAIL {error}", file=sys.stderr, flush=True)
        sys.exit(1)
