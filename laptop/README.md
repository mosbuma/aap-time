# JIP show runner (Phase 2)

**Python 3** reads **`NEXT`** / **`HIDE`** from the Arduino at **115200** and drives **mpv** on the beamer via **JSON IPC** (see [PLAN.md](../PLAN.md)).

**End-to-end install (sketch upload + this app):** [README.md](../README.md).

## Setup (macOS)

```bash
brew install mpv
cd laptop
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
mkdir -p videos
# add clip_000.mp4, clip_001.mp4, ... (sorted order)
```

**Displays:** System Settings → Displays → **Extend**. Note which index is the projector; pass **`--fs-screen`** (often **`1`** if **`0`** is the built-in).

**Bench D4 tact:** Press = **`NEXT`** (motor stops), **release** = **`HIDE`**. A held button never sends **`HIDE`**.

## Run

```bash
source .venv/bin/activate
python3 jip_show.py
# or: python3 jip_show.py --port /dev/cu.usbmodem1101 --fs-screen 1 --videos ./videos
```

Start **`mpv`** first is handled by the script; it spawns **mpv once** and sends IPC for each event.

## Options

| Flag | Default | Meaning |
| --- | --- | --- |
| `--port` | first `/dev/cu.usbmodem*` | USB serial device |
| `--videos` | `./videos` next to `jip_show.py` | Folder of `*.mp4` (lexicographic order) |
| `--fs-screen` | `1` | `mpv --fs-screen` (projector index) |
| `--baud` | `115200` | Match Arduino sketch |
| `--ipc-socket` | `/tmp/jip-mpv-ipc.sock` | mpv IPC Unix socket path |
