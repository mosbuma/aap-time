# JIP rail controller — bench firmware

This folder contains the **Arduino sketch** used to drive a **relay + small DC motor** on the workbench. It builds on **DFRobot Beginner Kit experiment 13** (relay + motor), with changes for our hardware and show logic.

**Status:** Verified on hardware; **Phase 2** serial includes **`NEXT`** (ready **press**) and **`HIDE`** (ready **release**).

**Install firmware + laptop app in one flow:** [README.md](../../README.md) (project root).  
Full design: [PLAN.md](../../PLAN.md). Parts / tutorials: [reference/links.md](../../reference/links.md).

---

## What this sketch does

| Control | Pin | Action |
|--------|-----|--------|
| **START** | **D2** | Momentary press → relay **ON** (motor runs). Same role as the experiment 13 “button”; kit-style wiring (**HIGH** = pressed). |
| **Relay** | **D3** | Logic output to the **custom** relay module (not necessarily the kit relay). |
| **STOP / ready** | **D4** | **Press** → relay **OFF**, `NEXT n`. **Release** → `HIDE`. Same wiring as **START**. **Held** button ⇒ no **`HIDE`** until release. |

- **No separate LED** on the breadboard for “motor on”: the **relay module’s built-in LED** shows when the coil is driven.
- **USB Serial** at **115200 baud**: each **D4 press** prints `NEXT 0`, `NEXT 1`, …; each **D4 release** (after a press) prints `HIDE`. See **`laptop/jip_show.py`**.

Debounce (**50 ms**) follows the same pattern as DFRobot [project_13.ino](https://github.com/DFRobot/Beginner-Kit-for-Arduino/blob/master/Sample%20Code/project_13/project_13.ino).

If the relay is **inverted** (motor runs when it should be off), change `#define RELAY_ACTIVE_LOW` at the top of the sketch (`0` vs `1`) and re-upload.

---

## Hardware recap (as built)

- **DFRobot-style** digital inputs: **INPUT** with external **pull-down** (e.g. 10 kΩ to GND), button between **pin** and **5 V** so **released = LOW**, **pressed = HIGH**.
- **D2**: existing experiment **START** button.
- **D4**: second button = **STOP** / stand-in for the future **ready** sensor.
- **D3**: **custom** relay module (signal + power per module silkscreen).
- Motor circuit on the relay **contacts** as in experiment 13 (e.g. **5 V** rail, **COM**/**NO**, motor return to **GND**); use a supply that matches the motor if it is not trivial.

Reference video (stack / layout ideas): [YouTube — DFRobot R3 + proto shield](https://www.youtube.com/watch?v=GxvDaQeCQKw).

---

## Steps completed so far

1. **Clone or copy** this repo (or copy only the `jip_rail_controller` folder).
2. **Arduino IDE**: install board support for the Arduino you use (e.g. **Arduino AVR Boards** for an Uno-compatible **DFRobot R3**).
3. **Open** `jip_rail_controller.ino` (**File → Open**). The IDE expects the folder name to match the `.ino` filename.
4. **Select board** (e.g. **Arduino Uno**) and the correct **COM port** (**Tools** menu).
5. **Upload** the sketch (**Sketch → Upload** or arrow button).
6. **Test**: press **START** → motor runs, relay LED on; press **STOP** → motor stops, relay LED off.
7. **Optional:** **Serial Monitor** at **115200** — press **D4** → `NEXT …`, release **D4** → `HIDE`.

---

## Next (when you pick it up again)

- Swap **D4** tact for the real **ready** sensor (must **clear** when the card moves so **`HIDE`** fires).
- **Rail build:** relay + 5 V bench → **XY‑160D** + **12 V wiper** per [PLAN.md](../../PLAN.md).
- **Phase 2:** [laptop/README.md](../../laptop/README.md) — **`jip_show.py`**, **`laptop/videos/*.mp4`**.
