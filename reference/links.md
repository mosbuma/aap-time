# Motor control example (DC motor + driver + Arduino)

https://racheldebarros.com/arduino-projects/turn-on-dc-motor-with-pir-motion-sensor-and-arduino/

# Ordered parts (ELEGOO UNO R3 Project Most Complete Starter Kit, EL-KIT-001, ASIN B01IHCCKKK)

https://www.amazon.nl/gp/aw/d/B01IHCCKKK/?_encoding=UTF8&pd_rd_plhdr=t&aaxitk=0282a0293bd9b46d1d0e98c298da738f&hsa_cr_id=0&qid=1775731107&sr=1-1-e0fa1fdd-d857-4087-adda-5bd576b25987&aref=z3vsi5oc4N&ref_=sbx_s_sparkle_sbtcd_asin_0_title&pd_rd_w=TqvgG&content-id=amzn1.sym.dfbc127b-5092-488c-8870-08a6275f0ae1%3Aamzn1.sym.dfbc127b-5092-488c-8870-08a6275f0ae1&pf_rd_p=dfbc127b-5092-488c-8870-08a6275f0ae1&pf_rd_r=HD0T8G3PFQJYSNFBFCEQ&pd_rd_wg=duqda&pd_rd_r=f4119d03-46bc-4521-9b3e-eae9333befe6

# Kit product page & tutorial / code downloads (ELEGOO)

https://www.elegoo.com/products/elegoo-uno-most-complete-starter-kit

https://www.elegoo.com/blogs/arduino-projects/elegoo-uno-r3-project-the-most-complete-starter-kit-tutorial

# Detailed parts list for EL-KIT-001 (retailer listing; cross-check with your box)

https://www.newegg.com/elegoo-el-kit-001-accessories/p/293-001C-00001

# Motor driver (ordered): XY-160D dual DC motor driver module

https://amzn.to/46rwGHT

# Rail actuator (chosen): 12 V automotive wiper motor

Use a **12 V car wiper motor** coupled to the rail (gear / belt / friction wheel per mechanics). Same class of motor as in the reference tutorial (high torque, expect **several amps** under load—size **12 V PSU**, **XY‑160D**, and **wiring** accordingly). Add a product or salvage note URL here when you lock the exact unit.

https://racheldebarros.com/arduino-projects/turn-on-dc-motor-with-pir-motion-sensor-and-arduino/

# Ready sensor (TBD — add product / wiring link when chosen)

**Role:** one digital **ready** input; **active** = **stop motor**, then laptop plays **next** looping video. **Step** starts motor again until **ready** fires once more. **Limit switches:** not used.

(Add URL for the specific sensor module or circuit here.)

# Bench test: DFRobot Experiment 13 + custom relay

Official **project 13** sample (Arduino IDE — open or copy from repo):

https://github.com/DFRobot/Beginner-Kit-for-Arduino/blob/master/Sample%20Code/project_13/project_13.ino

Video (**R3 + proto shield** style stack): https://www.youtube.com/watch?v=GxvDaQeCQKw

**Experiment 13 pinout:** **D2** = kit button, **D3** = relay. This project keeps that layout but uses a **custom relay module** on **D3** (set `RELAY_ACTIVE_LOW` / wiring per your board). **READY** is extra on **D4** (same input style as kit button: pull‑down + **HIGH** = active). Firmware (from repo root): `arduino/jip_rail_controller/jip_rail_controller.ino`

https://github.com/DFRobot/Beginner-Kit-for-Arduino/blob/master/Sample%20Code/project_13/project_13.ino
