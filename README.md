# JIP — curtain rail + video sync

**Arduino** motion + **READY** gating and **Mac** looping **mp4** over USB serial (**`NEXT`** / **`HIDE`**). Protocol and choreography: **[PLAN.md](PLAN.md)**.

## Behaviour (short)

- **Boot:** motor off, no video; **READY** may be any state.
- **STEP (D2):** if a video is playing → **`HIDE`** immediately; motor **on**; wait **READY** **open** (skip if already open), then **closed** → motor **off**, **`NEXT`** → **one** clip loops on the beamer.
- **READY** does **not** stop the video; only **STEP** does, then the rail moves to the next panel.

## Quick reference

| Piece | Location |
| --- | --- |
| Firmware + pins | [arduino/jip_rail_controller/](arduino/jip_rail_controller/) |
| Show app | [laptop/](laptop/) — `jip_show.py`, [laptop/README.md](laptop/README.md) |
| Plan | [PLAN.md](PLAN.md) |
| Parts / links | [reference/links.md](reference/links.md) |

## Install and run (reference)

**Arduino:** Open `arduino/jip_rail_controller/jip_rail_controller.ino`, upload. Serial Monitor **115200**: **STEP** starts the move; after **READY** open→close you see **`NEXT`**; **STEP** while “showing” sends **`HIDE`** first (see sketch + [PLAN.md](PLAN.md)).

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

Use **`--port /dev/cu.usbmodem…`** if needed.

**Bench test with tact on D4:** **STEP** → open **D4** → close **D4** → next clip loops; **STEP** again → video stops, motor runs, repeat open/close for the following clip.
