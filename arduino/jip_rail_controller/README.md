# JIP rail controller — bench firmware (Phase 1)

This folder contains the **Arduino sketch** used to drive a **relay + small DC motor** on the workbench. It builds on **DFRobot Beginner Kit experiment 13** (relay + motor), with changes for our hardware and show logic.

**Status:** Sketch loaded in **Arduino IDE**, uploaded to the board, and **verified working** on hardware.

For the full installation vision (rail, XY-160D, laptop video), see the project [PLAN.md](../../PLAN.md). Part links and tutorials: [reference/links.md](../../reference/links.md).

---

## What this sketch does

| Control | Pin | Action |
|--------|-----|--------|
| **START** | **D2** | Momentary press → relay **ON** (motor runs). Same role as the experiment 13 “button”; kit-style wiring (**HIGH** = pressed). |
| **Relay** | **D3** | Logic output to the **custom** relay module (not necessarily the kit relay). |
| **STOP / ready** | **D4** | Momentary press → relay **OFF** (motor stops). Wired like the **START** button (pull-down + switch to 5 V). Prints a **Serial** line for later laptop use. |

- **No separate LED** on the breadboard for “motor on”: the **relay module’s built-in LED** shows when the coil is driven.
- **USB Serial** at **115200 baud**: after each **STOP** press the board prints `NEXT 0`, `NEXT 1`, … (Phase 2 can use this to advance video).

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
7. **Optional:** **Tools → Serial Monitor** at **115200** and press **STOP** a few times; confirm lines like `NEXT 0`, `NEXT 1`, …

---

## Next (when you pick it up again)

- Swap **STOP** from a button to the real **ready** sensor when the mechanical rig exists.
- **Rail build:** move from relay + 5 V bench motor to **XY‑160D** + **12 V wiper** per [PLAN.md](../../PLAN.md).
- **Phase 2:** laptop reads **Serial** and plays the next looping clip.
