# JIP — curtain rail + video sync

Motor / ready logic on **Arduino**, looping **mp4** on a **Mac** + beamer over **USB serial** (`NEXT` / `HIDE`). Full design: [PLAN.md](PLAN.md).

---

## Install and run

Do this on the **same Mac** that will drive the **HDMI** projector. Leave the Arduino plugged in over **USB** for the whole show.

### 1. Arduino firmware (`.ino`)

1. Install [Arduino IDE](https://www.arduino.cc/en/software) and board support for your board (e.g. **Arduino AVR Boards** for Uno / DFRobot R3).
2. Open the sketch: **File → Open** → `arduino/jip_rail_controller/jip_rail_controller.ino` (the folder name must match the `.ino` name).
3. **Tools → Board** — select your board (e.g. **Arduino Uno**).
4. **Tools → Port** — pick the USB port for the Arduino.
5. **Sketch → Upload** (or the upload arrow).
6. **Tools → Serial Monitor** — set **115200 baud**. Press **D4** (ready): you should see `NEXT 0`, … Release **D4**: you should see `HIDE`.

Pin map, relay, and bench wiring: [arduino/jip_rail_controller/README.md](arduino/jip_rail_controller/README.md).

### 2. Laptop app (`jip_show.py`)

On **macOS** (see [PLAN.md](PLAN.md)): **Homebrew** **mpv**, **Python 3**, **pyserial**.

```bash
brew install mpv
cd laptop
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
mkdir -p videos
# Copy or name clips: clip_000.mp4, clip_001.mp4, ... (lexicographic order)
```

**Display:** **System Settings → Displays** — use **extended** desktop and note the **projector** screen index. If fullscreen lands on the wrong display, change **`--fs-screen`** (often **`1`** when the built-in is **`0`**).

**Run** (Arduino still on USB):

```bash
cd laptop
source .venv/bin/activate
python3 jip_show.py --fs-screen 1
```

If several devices appear under `/dev/cu.*`, pass an explicit port, e.g. `python3 jip_show.py --port /dev/cu.usbmodem1101 --fs-screen 1`.

More flags and behaviour: [laptop/README.md](laptop/README.md).

### 3. Quick bench check

1. Upload the sketch; start `jip_show.py` with at least two **`*.mp4`** files in `laptop/videos/`.
2. **D4 press** — should advance to the next clip on the projector ( **`NEXT`** ).
3. **D4 release** — clip should stop / idle ( **`HIDE`** ).
4. **D2** — motor / relay only; it does not send video commands.

---

## Repo layout

| Path | Role |
| --- | --- |
| `PLAN.md` | Full installation and protocol |
| `arduino/jip_rail_controller/` | Firmware + [README](arduino/jip_rail_controller/README.md) |
| `laptop/` | `jip_show.py`, `requirements.txt`, [README](laptop/README.md), `videos/*.mp4` |
| `reference/links.md` | Parts and tutorials |
