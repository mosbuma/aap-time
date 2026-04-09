/*
 * JIP rail / show controller — Phase 1 (Arduino IDE)
 *
 * Bench hardware: DFRobot Beginner Kit *Experiment 13* (Motor Fan / relay + motor),
 * official sample:
 *   https://github.com/DFRobot/Beginner-Kit-for-Arduino/blob/master/Sample%20Code/project_13/project_13.ino
 * Video walkthrough (proto shield stack):
 *   https://www.youtube.com/watch?v=GxvDaQeCQKw
 *
 * Pinout matched to project_13.ino:
 *   D2 = step (same as kit "button" — external pull-down, HIGH = pressed)
 *   D3 = relay control (same as kit; *custom relay module*: wire IN/active level per *your* board)
 *
 * Not in the original lesson: READY on D4 — use the *same style* as the kit button
 * (220 Ω + pull-down to GND on the DFRobot schematic) so HIGH = ready active.
 *
 * Final install (rail): set USE_RELAY_TEST to 0 for XY-160D; motor patterns follow:
 *   https://racheldebarros.com/arduino-projects/turn-on-dc-motor-with-pir-motion-sensor-and-arduino/
 */

#ifndef USE_RELAY_TEST
#define USE_RELAY_TEST 1
#endif

// 1 = DFRobot experiment 13 style: INPUT + external pull-down, HIGH = pressed (50 ms debounce like sample)
// 0 = breadboard tactile with INPUT_PULLUP, LOW = pressed
#ifndef DFRobot_EXP13_INPUTS
#define DFRobot_EXP13_INPUTS 1
#endif

// project_13.ino: int buttonPin = 2; int relayPin = 3;
static const uint8_t kPinStep = 2;
static const uint8_t kPinRelayIn = 3;
static const uint8_t kPinReady = 4; // added for show cycle (not in original project 13)

// XY-160D (final rig): ENA ~6, IN1 7, IN2 8 — tutorial convention
static const uint8_t kPinEna = 6;
static const uint8_t kPinIn1 = 7;
static const uint8_t kPinIn2 = 8;

// Many opto relay *modules* are active-LOW on IN. DFRobot kit relay on D3 may be active-HIGH — try 0 if coil never clicks right.
#ifndef RELAY_ACTIVE_LOW
#define RELAY_ACTIVE_LOW 0
#endif

static const uint8_t kMotorPwmRun = 220;

#if DFRobot_EXP13_INPUTS
static const unsigned long kDebounceMs = 50; // project_13 debounceDelay
#else
static const unsigned long kDebounceMs = 40;
#endif

static const uint32_t kSerialBaud = 115200;
static const char kSerialNextToken[] = "NEXT";

// ---------------------------------------------------------------------------

enum RunState {
  ST_IDLE = 0,
  ST_RUNNING,
  ST_WAIT_READY_RELEASE,
};

static RunState g_state = ST_IDLE;

struct DebouncedInput {
  uint8_t pin;
  bool stableLevel;
  bool lastRaw;
  unsigned long lastChangeMs;
};

static DebouncedInput g_step;
static DebouncedInput g_ready;

static bool g_prevStepStable = false;
static bool g_prevReadyStable = false;

static uint16_t g_clipIndex = 0;

static bool isPressed(bool stable) {
#if DFRobot_EXP13_INPUTS
  return stable == HIGH;
#else
  return stable == LOW;
#endif
}

static void debouncerInit(DebouncedInput *d) {
  d->lastRaw = digitalRead(d->pin);
  d->stableLevel = d->lastRaw;
  d->lastChangeMs = millis();
}

static void debouncerUpdate(DebouncedInput *d) {
  const bool raw = digitalRead(d->pin);
  const unsigned long now = millis();
  if (raw != d->lastRaw) {
    d->lastRaw = raw;
    d->lastChangeMs = now;
  }
  if ((now - d->lastChangeMs) >= kDebounceMs) {
    d->stableLevel = d->lastRaw;
  }
}

static void motorStop() {
#if USE_RELAY_TEST
#  if RELAY_ACTIVE_LOW
  digitalWrite(kPinRelayIn, HIGH);
#  else
  digitalWrite(kPinRelayIn, LOW);
#  endif
#else
  digitalWrite(kPinIn1, LOW);
  digitalWrite(kPinIn2, LOW);
  analogWrite(kPinEna, 0);
#endif
}

static void motorRunForward() {
#if USE_RELAY_TEST
#  if RELAY_ACTIVE_LOW
  digitalWrite(kPinRelayIn, LOW);
#  else
  digitalWrite(kPinRelayIn, HIGH);
#  endif
#else
  digitalWrite(kPinIn1, HIGH);
  digitalWrite(kPinIn2, LOW);
  analogWrite(kPinEna, kMotorPwmRun);
#endif
}

static void emitNextLine() {
  Serial.print(kSerialNextToken);
  Serial.print(' ');
  Serial.println(g_clipIndex);
  g_clipIndex++;
}

void setup() {
  g_step.pin = kPinStep;
  g_ready.pin = kPinReady;

  Serial.begin(kSerialBaud);

#if DFRobot_EXP13_INPUTS
  pinMode(kPinStep, INPUT);
  pinMode(kPinReady, INPUT);
#else
  pinMode(kPinStep, INPUT_PULLUP);
  pinMode(kPinReady, INPUT_PULLUP);
#endif

#if USE_RELAY_TEST
  pinMode(kPinRelayIn, OUTPUT);
#else
  pinMode(kPinEna, OUTPUT);
  pinMode(kPinIn1, OUTPUT);
  pinMode(kPinIn2, OUTPUT);
#endif
  motorStop();

  debouncerInit(&g_step);
  debouncerInit(&g_ready);
  g_prevStepStable = g_step.stableLevel;
  g_prevReadyStable = g_ready.stableLevel;

  Serial.println(F("jip_rail_controller Phase1"));
  Serial.print(F("USE_RELAY_TEST="));
  Serial.print(USE_RELAY_TEST);
  Serial.print(F(" DFRobot_EXP13_INPUTS="));
  Serial.println(DFRobot_EXP13_INPUTS);
}

void loop() {
  debouncerUpdate(&g_step);
  debouncerUpdate(&g_ready);

  const bool stepStable = g_step.stableLevel;
  const bool readyStable = g_ready.stableLevel;

  const bool stepEdgePress =
      (!isPressed(g_prevStepStable) && isPressed(stepStable));
  g_prevStepStable = stepStable;

  const bool readyEdgePress =
      (!isPressed(g_prevReadyStable) && isPressed(readyStable));
  g_prevReadyStable = readyStable;

  switch (g_state) {
    case ST_IDLE:
      motorStop();
      if (stepEdgePress && !isPressed(readyStable)) {
        motorRunForward();
        g_state = ST_RUNNING;
      }
      break;

    case ST_RUNNING:
      if (readyEdgePress) {
        motorStop();
        emitNextLine();
        g_state = ST_WAIT_READY_RELEASE;
      }
      break;

    case ST_WAIT_READY_RELEASE:
      motorStop();
      if (!isPressed(readyStable)) {
        g_state = ST_IDLE;
      }
      break;
  }
}
