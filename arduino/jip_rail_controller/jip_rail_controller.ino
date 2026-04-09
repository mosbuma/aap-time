/*
 * JIP rail controller — bench / experiment 13 style (custom relay on D3)
 *
 * Debounce pattern from DFRobot project 13:
 *   https://github.com/DFRobot/Beginner-Kit-for-Arduino/blob/master/Sample%20Code/project_13/project_13.ino
 *
 * Behaviour (see PLAN.md):
 *   D2 STEP — each press: if a video is playing, HIDE then motor on; else motor on.
 *     Then wait for D4 ready OPEN (LOW), then CLOSED (HIGH); on close stop motor and
 *     print NEXT <n> (one clip loops on laptop). Ignore STEP while motor is moving.
 *   D4 READY — LOW = open, HIGH = closed (in position). Used only while motor runs;
 *     does not send HIDE; releasing ready does not stop video.
 *
 * Wiring: kit-style pull-down on D2/D4 (released = LOW, pressed/closed = HIGH).
 */

const int pinStart = 2;
const int pinRelay = 3;
const int pinStop = 4;

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

static const uint8_t MOVE_IDLE = 0;
static const uint8_t MOVE_WAIT_OPEN = 1;
static const uint8_t MOVE_WAIT_CLOSE = 2;

static uint8_t movePhase = MOVE_IDLE;
static bool showingVideo = false;

static void relaySetMotorOn(bool on) {
#if RELAY_ACTIVE_LOW
  digitalWrite(pinRelay, on ? LOW : HIGH);
#else
  digitalWrite(pinRelay, on ? HIGH : LOW);
#endif
}

static void onStepPressed() {
  if (movePhase != MOVE_IDLE) {
    return;
  }
  if (showingVideo) {
    Serial.println(F("HIDE"));
    showingVideo = false;
  }
  relaySetMotorOn(true);
  if (stopState == HIGH) {
    movePhase = MOVE_WAIT_OPEN;
  } else {
    movePhase = MOVE_WAIT_CLOSE;
  }
}

void setup() {
  pinMode(pinStart, INPUT);
  pinMode(pinStop, INPUT);
  pinMode(pinRelay, OUTPUT);

  relaySetMotorOn(false);
  movePhase = MOVE_IDLE;
  showingVideo = false;

  startState = digitalRead(pinStart);
  stopState = digitalRead(pinStop);

  Serial.begin(115200);
  Serial.println(F("jip_rail_controller D2=STEP D4=ready NEXT/HIDE-on-step"));
}

void loop() {
  {
    int reading = digitalRead(pinStop);
    if (reading != lastStopReading) {
      lastStopDebounce = millis();
    }
    if ((millis() - lastStopDebounce) > debounceDelay) {
      if (reading != stopState) {
        stopState = reading;
      }
    }
    lastStopReading = reading;
  }

  if (movePhase == MOVE_WAIT_OPEN && stopState == LOW) {
    movePhase = MOVE_WAIT_CLOSE;
  }
  if (movePhase == MOVE_WAIT_CLOSE && stopState == HIGH) {
    relaySetMotorOn(false);
    Serial.print(F("NEXT "));
    Serial.println(clipIndex);
    clipIndex++;
    showingVideo = true;
    movePhase = MOVE_IDLE;
  }

  {
    int reading = digitalRead(pinStart);
    if (reading != lastStartReading) {
      lastStartDebounce = millis();
    }
    if ((millis() - lastStartDebounce) > debounceDelay) {
      if (reading != startState) {
        startState = reading;
        if (startState == HIGH) {
          onStepPressed();
        }
      }
    }
    lastStartReading = reading;
  }
}
