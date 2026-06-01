#include "traffic_light.h"

TrafficLight::TrafficLight()
    : cfg_{0, 0, 0, 0, 0, 0},
      phase_(PHASE_RED),
      mode_(MODE_NORMAL),
      phaseStartedAt_(0),
      blinkOn_(false),
      lastBlinkAt_(0),
      lastButtonReading_(HIGH),
      stableButtonState_(HIGH),
      lastDebounceAt_(0),
      waitingToStart_(false),
      startAt_(0),
      greenBlinkOn_(false),
      greenBlinkOnsDone_(0),
      greenBlinkLastToggleAt_(0) {}

void TrafficLight::begin(const TrafficLightConfig& cfg, unsigned long startDelayMs) {
  cfg_ = cfg;

  pinMode(cfg_.pinRed,    OUTPUT);
  pinMode(cfg_.pinYellow, OUTPUT);
  pinMode(cfg_.pinGreen,  OUTPUT);
  pinMode(cfg_.pinButton, INPUT_PULLUP);

  unsigned long now = millis();
  if (startDelayMs > 0) {
    // Show solid RED for startDelayMs, then begin the normal cycle.
    waitingToStart_ = true;
    startAt_        = now + startDelayMs;
    setLights(true, false, false);
  } else {
    enterNormalMode(now);
  }
}

void TrafficLight::update(unsigned long now) {
  // Button is polled in every state, including during the staggered-start
  // wait — a press there cancels the wait and enters emergency mode.
  if (pollButtonPressed(now)) {
    waitingToStart_ = false;
    if (mode_ == MODE_NORMAL) {
      enterEmergencyMode(now);
    } else {
      enterNormalMode(now);
    }
    return;
  }

  if (waitingToStart_) {
    if (now >= startAt_) {
      waitingToStart_ = false;
      enterNormalMode(now);
    }
    return;
  }

  if (mode_ == MODE_NORMAL) {
    updateNormal(now);
  } else {
    updateEmergency(now);
  }
}

void TrafficLight::setLights(bool r, bool y, bool g) {
  digitalWrite(cfg_.pinRed,    r ? HIGH : LOW);
  digitalWrite(cfg_.pinYellow, y ? HIGH : LOW);
  digitalWrite(cfg_.pinGreen,  g ? HIGH : LOW);
}

void TrafficLight::enterPhase(Phase p, unsigned long now) {
  phase_          = p;
  phaseStartedAt_ = now;

  switch (p) {
    case PHASE_RED:        setLights(true,  false, false); break;
    case PHASE_RED_YELLOW: setLights(true,  true,  false); break;
    case PHASE_GREEN:      setLights(false, false, true);  break;
    case PHASE_GREEN_BLINK:
      // Enter the blink phase in the "off" half so the user sees a clear
      // transition out of solid green. Reset blink counters/timer.
      setLights(false, false, false);
      greenBlinkOn_           = false;
      greenBlinkOnsDone_      = 0;
      greenBlinkLastToggleAt_ = now;
      break;
    case PHASE_YELLOW:     setLights(false, true,  false); break;
  }
}

void TrafficLight::enterNormalMode(unsigned long now) {
  mode_ = MODE_NORMAL;
  // Per spec §2.3: return from emergency always restarts at RED.
  enterPhase(PHASE_RED, now);
}

void TrafficLight::enterEmergencyMode(unsigned long now) {
  mode_         = MODE_EMERGENCY;
  blinkOn_      = true;
  lastBlinkAt_  = now;
  setLights(false, true, false);  // yellow on for the first half-period
}

unsigned long TrafficLight::phaseDuration(Phase p) const {
  switch (p) {
    case PHASE_RED:         return cfg_.redDurationMs;
    case PHASE_RED_YELLOW:  return RED_YELLOW_DURATION_MS;
    case PHASE_GREEN:       return cfg_.greenDurationMs;
    case PHASE_GREEN_BLINK: return 0;  // self-managed in updateGreenBlink
    case PHASE_YELLOW:      return YELLOW_DURATION_MS;
  }
  return 0;
}

void TrafficLight::updateNormal(unsigned long now) {
  // GREEN_BLINK has its own toggle-based timing, not a single phase timer.
  if (phase_ == PHASE_GREEN_BLINK) {
    updateGreenBlink(now);
    return;
  }

  if (now - phaseStartedAt_ < phaseDuration(phase_)) {
    return;  // current phase still active
  }

  // RED → RED_YELLOW → GREEN → GREEN_BLINK → YELLOW → RED
  Phase next;
  switch (phase_) {
    case PHASE_RED:        next = PHASE_RED_YELLOW;  break;
    case PHASE_RED_YELLOW: next = PHASE_GREEN;       break;
    case PHASE_GREEN:      next = PHASE_GREEN_BLINK; break;
    case PHASE_YELLOW:     next = PHASE_RED;         break;
    default:               next = PHASE_RED;         break;
  }
  enterPhase(next, now);
}

void TrafficLight::updateEmergency(unsigned long now) {
  if (now - lastBlinkAt_ < EMERGENCY_BLINK_INTERVAL_MS) {
    return;
  }
  blinkOn_     = !blinkOn_;
  lastBlinkAt_ = now;
  setLights(false, blinkOn_, false);
}

void TrafficLight::updateGreenBlink(unsigned long now) {
  // Wait the appropriate half-period for the current LED state.
  unsigned long interval = greenBlinkOn_ ? GREEN_BLINK_ON_MS : GREEN_BLINK_OFF_MS;
  if (now - greenBlinkLastToggleAt_ < interval) {
    return;
  }

  if (greenBlinkOn_) {
    // An on-period just finished. Count it and exit early if we're done so
    // we don't bother turning the LED off only to immediately swap to YELLOW.
    greenBlinkOnsDone_++;
    if (greenBlinkOnsDone_ >= GREEN_BLINK_COUNT) {
      enterPhase(PHASE_YELLOW, now);
      return;
    }
    greenBlinkOn_ = false;
    setLights(false, false, false);
  } else {
    greenBlinkOn_ = true;
    setLights(false, false, true);
  }
  greenBlinkLastToggleAt_ = now;
}

bool TrafficLight::pollButtonPressed(unsigned long now) {
  int reading = digitalRead(cfg_.pinButton);

  // Any change resets the debounce timer.
  if (reading != lastButtonReading_) {
    lastDebounceAt_   = now;
    lastButtonReading_ = reading;
  }

  // Wait until the reading has been stable long enough.
  if (now - lastDebounceAt_ < DEBOUNCE_DELAY_MS) {
    return false;
  }

  // Stable reading differs from the last committed state → commit it.
  if (reading != stableButtonState_) {
    stableButtonState_ = reading;
    // INPUT_PULLUP: pressed = LOW. Fire only on the press edge.
    if (stableButtonState_ == LOW) {
      return true;
    }
  }
  return false;
}
