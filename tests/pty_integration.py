"""Launch the simulator and monitor as independent processes against /dev/pts."""

import argparse
import json
from pathlib import Path
import select
import shlex
import subprocess
import sys
import time
import uuid


def stop(process):
    if process is None or process.poll() is not None:
        return
    process.terminate()
    try:
        process.wait(timeout=2)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait(timeout=2)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--scenario", choices=("normal", "malformed", "disconnect", "timeout"), required=True)
    parser.add_argument("--monitor", type=Path, required=True)
    parser.add_argument("--simulator", type=Path, required=True)
    parser.add_argument("--wrapper", default="")
    args = parser.parse_args()
    root = Path.cwd() / "artifacts" / f"{args.scenario}-{uuid.uuid4().hex[:10]}"
    root.mkdir(parents=True, exist_ok=False)
    monitor_log = root / "monitor.log"
    simulator_log = root / "simulator.log"
    sim = monitor = None
    passed = False
    with monitor_log.open("wb") as monitor_file, simulator_log.open("wb") as simulator_file:
        try:
            sim = subprocess.Popen([sys.executable, "-u", str(args.simulator), "--scenario", args.scenario],
                                   stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=simulator_file,
                                   text=True, bufsize=1)
            ready, _, _ = select.select([sim.stdout], [], [], 3)
            if not ready:
                raise RuntimeError("simulator did not publish PTY path")
            first = sim.stdout.readline().strip()
            if not first.startswith("READY /dev/pts/"):
                raise RuntimeError(f"invalid simulator readiness: {first}")
            simulator_file.write((first + "\n").encode())
            simulator_file.flush()
            device = first.split(" ", 1)[1]
            command = [*shlex.split(args.wrapper), str(args.monitor), "--device", device]
            if args.scenario == "malformed":
                command += ["--expect-invalid", "3"]
            elif args.scenario == "disconnect":
                command += ["--expect-data", "1", "--expect-disconnect"]
            elif args.scenario == "timeout":
                command += ["--expect-data", "0", "--expect-timeout"]
            monitor = subprocess.Popen(command, stdout=monitor_file, stderr=subprocess.STDOUT)
            deadline = time.monotonic() + 3
            while time.monotonic() < deadline:
                if "STATE RUNNING" in monitor_log.read_text(errors="replace"):
                    break
                if monitor.poll() is not None:
                    raise RuntimeError(f"monitor exited before opening PTY: {monitor_log.read_text()}")
                time.sleep(0.03)
            else:
                raise RuntimeError("monitor did not open PTY")
            sim.stdin.write("START\n")
            sim.stdin.flush()
            monitor.wait(timeout=7)
            if args.scenario != "timeout":
                sim.wait(timeout=3)
            log = monitor_log.read_text(errors="replace")
            required = ["EVENT OPEN path=/dev/pts/", "STATE RUNNING", "EVENT CLOSE fd_released=1", "result=PASS"]
            if args.scenario in ("normal", "malformed"):
                required += ["EVENT DATA seq=1 value=42", "EVENT DATA seq=2 value=43",
                             "EVENT COMMAND_SENT", "EVENT ACK seq=1 payload=PONG"]
            if args.scenario == "malformed":
                required += ["EVENT REJECT parser_total=", "EVENT REJECT sequence"]
            elif args.scenario == "disconnect":
                required += ["STATE DISCONNECTED", "EVENT DATA seq=1 value=42"]
            elif args.scenario == "timeout":
                required += ["STATE TIMEOUT"]
                if "EVENT DATA" in log:
                    raise RuntimeError("timeout accepted missing data")
            if monitor.returncode != 0 or (args.scenario != "timeout" and sim.returncode != 0) or any(x not in log for x in required):
                raise RuntimeError(f"monitor rc={monitor.returncode}, sim rc={sim.poll()}, missing={[x for x in required if x not in log]}\n{log}\n{simulator_log.read_text()}")
            passed = True
            print(f"{args.scenario}: PASS node={device} monitor_rc={monitor.returncode} sim_rc={sim.poll()}")
        finally:
            stop(monitor)
            stop(sim)
            if sim is not None and sim.stdout is not None:
                simulator_file.write(sim.stdout.read().encode())
                simulator_file.flush()
                sim.stdout.close()
            if sim is not None and sim.stdin is not None:
                sim.stdin.close()
            (root / "result.json").write_text(json.dumps({"scenario": args.scenario, "passed": passed,
                "monitor_exit": monitor.returncode if monitor else None,
                "simulator_exit": sim.returncode if sim else None}, indent=2) + "\n")
            print(f"evidence={root}")


if __name__ == "__main__":
    try:
        main()
    except Exception as error:
        print(f"INTEGRATION_FAIL {error}", file=sys.stderr)
        sys.exit(1)
