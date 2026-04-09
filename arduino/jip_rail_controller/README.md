# JIP rail controller — bench firmware

**DFRobot experiment 13** style: **D3** relay + **D2** STEP + **D4** READY. Full choreography: [PLAN.md](../../PLAN.md).

**Install + laptop:** [README.md](../../README.md).

---

## What this sketch does

| Control | Pin | Action |
|--------|-----|--------|
| **STEP** | **D2** | **Press**: if the laptop should be looping a clip after the last **`NEXT`**, send **`HIDE`** first; turn motor (**relay**) **on**; then wait for **READY** **open** (LOW), then **closed** (HIGH); on close turn motor **off** and print **`NEXT <n>`**. Further **STEP** presses are ignored until that cycle finishes. |
| **Relay** | **D3** | Motor run (**`RELAY_ACTIVE_LOW`** for common opto modules). |
| **READY** | **D4** | **Only while motor runs:** must go **open** (LOW) then **closed** (HIGH) to end the move and emit **`NEXT`**. **READY does not** send **`HIDE`** and does not stop video; only **STEP** stops playback and starts the next move. |

Wiring: kit-style **pull-down** on **D2** / **D4** (**released = LOW**, **pressed / closed = HIGH**).

**USB serial** **115200**: **`HIDE`** on **STEP** when a show segment is active; **`NEXT n`** when **READY** closes after the open→close sequence. **`laptop/jip_show.py`** consumes both.

Debounce **50 ms** (same pattern as DFRobot [project_13.ino](https://github.com/DFRobot/Beginner-Kit-for-Arduino/blob/master/Sample%20Code/project_13/project_13.ino)).

If the relay is inverted, set **`RELAY_ACTIVE_LOW`** to `0` or `1` at the top of the sketch and re-upload.

---

## Hardware recap (as built)

- **D2**: STEP button.
- **D4**: READY (bench tact or future sensor — must **open** then **close** per move).
- **D3**: relay module; motor on **NO/COM** per your layout.
- Motor supply appropriate to the motor; common **GND** with Arduino logic.

---

## Bench test

1. Upload the sketch; open Serial Monitor **115200**.
2. **STEP** → motor on; **open D4** (if it was closed); **close D4** → motor off, **`NEXT 0`** (then **`NEXT 1`**, … on later cycles).
3. With **`jip_show.py`** running and **`videos/*.mp4`**: first cycle starts **clip_000**; **STEP** again → **`HIDE`** then motor — after **open/close D4**, next clip loads.

---

## Next

- Replace **D4** tact with the real **READY** switch/sensor (same **open**/**closed** meaning: **HIGH** = in position on this bench).
- Rail: relay + 5 V bench → **XY‑160D** + **12 V** per [PLAN.md](../../PLAN.md).
