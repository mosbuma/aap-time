/*
 * JIP rail controller — bench / experiment 13 style (custom relay on D3)
 *
 * Debounce pattern from DFRobot project 13:
 *   https://github.com/DFRobot/Beginner-Kit-for-Arduino/blob/master/Sample%20Code/project_13/project_13.ino
 *
 * No separate lesson LED on the print — use the relay module’s built-in LED as status.
 *
 * Same button wiring as the kit (pull-down to GND, switch to 5 V when pressed → HIGH):
 *   D2 = START — momentary press turns motor / relay ON.
 *   D4 = STOP / ready — press turns motor / relay OFF and prints NEXT; release prints HIDE
 *       (Phase 2 laptop blanks video on HIDE). Use a real sensor that clears when the card moves.
 */

const int pinStart = 2;
const int pinRelay = 3;
const int pinStop = 4;

// 0 = HIGH on D3 = relay ON (many kit / discrete boards)
// 1 = LOW on D3 = relay ON (common opto relay modules)
#ifndef RELAY_ACTIVE_LOW
#define RELAY_ACTIVE_LOW 0
#endif

const long debounceDelay = 50;

int startState;
int lastStartReading = LOW;
unsigned long lastStartDebounce = 0;

int stopState;
int lastStopReading = LOW;
unsigned long lastStopDebounce = 0;

uint16_t clipIndex = 0;

// After NEXT, the next ready release emits HIDE (no HIDE at boot or without a prior NEXT).
static bool armedForHide = false;

static void relaySetMotorOn(bool on) {
#if RELAY_ACTIVE_LOW
  digitalWrite(pinRelay, on ? LOW : HIGH);
#else
  digitalWrite(pinRelay, on ? HIGH : LOW);
#endif
}

static void onStopPressed() {
  relaySetMotorOn(false);
  Serial.print(F("NEXT "));
  Serial.println(clipIndex);
  clipIndex++;
  armedForHide = true;
}

static void onReadyReleased() {
  if (!armedForHide) {
    return;
  }
  Serial.println(F("HIDE"));
  armedForHide = false;
}

void setup() {
  pinMode(pinStart, INPUT);
  pinMode(pinStop, INPUT);
  pinMode(pinRelay, OUTPUT);

  relaySetMotorOn(false);
  startState = digitalRead(pinStart);
  stopState = digitalRead(pinStop);

  Serial.begin(115200);
  Serial.println(F("jip_rail_controller D2=START D4=ready NEXT/HIDE D3=relay"));
}

void loop() {
  {
    int reading = digitalRead(pinStart);
    if (reading != lastStartReading) {
      lastStartDebounce = millis();
    }
    if ((millis() - lastStartDebounce) > debounceDelay) {
      if (reading != startState) {
        startState = reading;
        if (startState == HIGH) {
          relaySetMotorOn(true);
        }
      }
    }
    lastStartReading = reading;
  }

  {
    int reading = digitalRead(pinStop);
    if (reading != lastStopReading) {
      lastStopDebounce = millis();
    }
    if ((millis() - lastStopDebounce) > debounceDelay) {
      if (reading != stopState) {
        stopState = reading;
        if (stopState == HIGH) {
          onStopPressed();
        } else {
          onReadyReleased();
        }
      }
    }
    lastStopReading = reading;
  }
}
