# JIP — curtain rail + video sync

**Arduino** motion/ready logic and **Mac** looping **mp4** over USB serial (**`NEXT`** / **`HIDE`**). Design, protocol, and what is left to tune: **[PLAN.md](PLAN.md)**.

## Status

**Firmware and laptop app are installed and running.** The remaining gap is **sequence and button combinations**: aligning **step (D2)**, **ready (D4)** edges, **`HIDE`**/**`NEXT`** ordering, and **re-arm** with the intended show flow. See [PLAN.md — Current status](PLAN.md#current-status) and [Remaining work](PLAN.md#remaining-work-priority).

## Quick reference

| Piece | Location |
| --- | --- |
| Firmware + pins | [arduino/jip_rail_controller/](arduino/jip_rail_controller/) |
| Show app | [laptop/](laptop/) — `jip_show.py`, [laptop/README.md](laptop/README.md) |
| Full plan | [PLAN.md](PLAN.md) |
| Parts / links | [reference/links.md](reference/links.md) |

## Install and run (reference)

**Arduino:** Open `arduino/jip_rail_controller/jip_rail_controller.ino`, select board/port, **Upload**. Serial Monitor **115200**: **D4** press → **`NEXT`**, release → **`HIDE`** (when armed).

**Laptop:**

```bash
brew install mpv
cd laptop
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
mkdir -p videos   # clip_000.mp4, clip_001.mp4, …
python3 jip_show.py --fs-screen 1
```

Use **`--port /dev/cu.usbmodem…`** if needed. **Displays:** extended desktop; fix **`--fs-screen`** if fullscreen hits the wrong screen.

**Bench check:** With two **mp4** files, **D4** should advance/stop video; **D2** drives motor/relay only (no serial show lines).
