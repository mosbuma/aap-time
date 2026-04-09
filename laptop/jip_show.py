#!/usr/bin/env python3
"""
JIP show runner — USB serial from Arduino drives mpv on the beamer via JSON IPC.

Arduino: HIDE on STEP while video active; NEXT when READY completes after motor run.
See ../PLAN.md. Intended for macOS (M2 Air + HDMI); mpv from Homebrew.
"""

from __future__ import annotations

import argparse
import glob
import json
import os
import re
import shutil
import signal
import socket
import subprocess
import sys
import tempfile
import time
from typing import List, Optional

try:
    import serial
except ImportError:
    print("Install deps: pip install -r requirements.txt", file=sys.stderr)
    raise

RE_NEXT = re.compile(r"^NEXT\s+(\d+)\s*$", re.IGNORECASE)
RE_HIDE = re.compile(r"^HIDE\s*$", re.IGNORECASE)


def find_default_serial_port() -> Optional[str]:
    candidates = sorted(glob.glob("/dev/cu.usbmodem*"))
    return candidates[0] if candidates else None


def build_playlist(video_dir: str) -> List[str]:
    paths = sorted(glob.glob(os.path.join(video_dir, "*.mp4")))
    return [os.path.abspath(p) for p in paths]


class MpvIpc:
    """One-shot Unix-socket client for mpv's --input-ipc-server (one JSON command per call)."""

    def __init__(self, sock_path: str) -> None:
        self._sock_path = sock_path

    def command(self, *args: object) -> None:
        payload = {"command": list(args)}
        raw = json.dumps(payload, separators=(",", ":")) + "\n"
        deadline = time.monotonic() + 2.0
        while not os.path.exists(self._sock_path) and time.monotonic() < deadline:
            time.sleep(0.05)
        if not os.path.exists(self._sock_path):
            raise RuntimeError(f"mpv IPC socket missing: {self._sock_path}")

        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
            s.settimeout(2.0)
            s.connect(self._sock_path)
            s.sendall(raw.encode("utf-8"))
            s.shutdown(socket.SHUT_WR)
            try:
                while True:
                    chunk = s.recv(8192)
                    if not chunk:
                        break
            except socket.timeout:
                pass


def start_mpv(
    sock_path: str,
    fs_screen: int,
) -> subprocess.Popen:
    exe = shutil.which("mpv")
    if not exe:
        raise RuntimeError("mpv not found in PATH (brew install mpv)")

    try:
        os.unlink(sock_path)
    except FileNotFoundError:
        pass

    cmd = [
        exe,
        "--idle=yes",
        "--force-window=immediate",
        "--loop-file=inf",
        "--fullscreen",
        "--native-fs=yes",
        f"--fs-screen={fs_screen}",
        f"--input-ipc-server={sock_path}",
        "--no-config",
        "--no-terminal",
        "--really-quiet",
    ]
    return subprocess.Popen(cmd, stdin=subprocess.DEVNULL)


def parse_line(line: str) -> Optional[str]:
    line = line.strip()
    if RE_HIDE.match(line):
        return "HIDE"
    if RE_NEXT.match(line):
        return "NEXT"
    return None


def main() -> int:
    script_dir = os.path.dirname(os.path.abspath(__file__))
    default_videos = os.path.join(script_dir, "videos")

    ap = argparse.ArgumentParser(description="JIP serial → mpv show control")
    ap.add_argument(
        "--port",
        default=None,
        help="Serial device (default: first /dev/cu.usbmodem*)",
    )
    ap.add_argument(
        "--videos",
        default=default_videos,
        help=f"Directory of *.mp4 (default: {default_videos})",
    )
    ap.add_argument(
        "--fs-screen",
        type=int,
        default=1,
        help="mpv fullscreen display index (0 = built-in on many Macs)",
    )
    ap.add_argument(
        "--baud",
        type=int,
        default=115200,
    )
    ap.add_argument(
        "--ipc-socket",
        default=os.path.join(tempfile.gettempdir(), "jip-mpv-ipc.sock"),
        help="Unix socket path for mpv JSON IPC",
    )
    args = ap.parse_args()

    port = args.port or find_default_serial_port()
    if not port:
        print("No serial port: plug in Arduino or pass --port", file=sys.stderr)
        return 1

    playlist = build_playlist(args.videos)
    if not playlist:
        print(
            f"No .mp4 in {args.videos!r} — add clips or pass --videos",
            file=sys.stderr,
        )
        return 1

    mpv_proc = start_mpv(args.ipc_socket, args.fs_screen)
    ipc = MpvIpc(args.ipc_socket)
    playlist_index = -1

    def load_clip(path: str) -> None:
        # Force fullscreen after each load; mpv can otherwise reopen as a small window
        # after stop/load cycles on some macOS display setups.
        ipc.command("loadfile", path, "replace")
        ipc.command("set_property", "fullscreen", True)

    def shutdown(signum: Optional[int] = None, _frame: object = None) -> None:
        try:
            mpv_proc.terminate()
            mpv_proc.wait(timeout=3)
        except Exception:
            try:
                mpv_proc.kill()
            except Exception:
                pass

    signal.signal(signal.SIGINT, shutdown)
    signal.signal(signal.SIGTERM, shutdown)

    try:
        ser = serial.Serial(port, args.baud, timeout=0.2)
    except serial.SerialException as e:
        print(f"Serial open failed: {e}", file=sys.stderr)
        shutdown()
        return 1

    print(f"Serial {port} @ {args.baud}", file=sys.stderr)
    print(f"Playlist {len(playlist)} files, mpv screen {args.fs_screen}", file=sys.stderr)

    try:
        while mpv_proc.poll() is None:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode("utf-8", errors="replace").strip()
            if not line:
                continue

            kind = parse_line(line)
            if kind == "HIDE":
                try:
                    ipc.command("stop")
                except Exception as e:
                    print(f"HIDE (stop): {e}", file=sys.stderr)
            elif kind == "NEXT":
                playlist_index += 1
                if playlist_index >= len(playlist):
                    print(
                        f"NEXT but playlist exhausted (index {playlist_index})",
                        file=sys.stderr,
                    )
                    continue
                path = playlist[playlist_index]
                try:
                    load_clip(path)
                except Exception as e:
                    print(f"NEXT loadfile: {e}", file=sys.stderr)
    finally:
        ser.close()
        shutdown()

    return 0


if __name__ == "__main__":
    sys.exit(main())
