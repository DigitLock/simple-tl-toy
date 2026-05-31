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
      lastDebounceAt_(0) {}

void TrafficLight::begin(const TrafficLightConfig& cfg) {
  cfg_ = cfg;

  pinMode(cfg_.pinRed,    OUTPUT);
  pinMode(cfg_.pinYellow, OUTPUT);
  pinMode(cfg_.pinGreen,  OUTPUT);
  pinMode(cfg_.pinButton, INPUT_PULLUP);

  // Start immediately in normal mode at RED, per spec §2.2.
  enterNormalMode(millis());
}

void TrafficLight::update(unsigned long now) {
  // Button always polled first: a press toggles mode regardless of phase.
  if (pollButtonPressed(now)) {
    if (mode_ == MODE_NORMAL) {
      enterEmergencyMode(now);
    } else {
      enterNormalMode(now);
    }
    return;  // mode just changed; nothing else to do this tick
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
    case PHASE_RED:        return cfg_.redDurationMs;
    case PHASE_RED_YELLOW: return RED_YELLOW_DURATION_MS;
    case PHASE_GREEN:      return cfg_.greenDurationMs;
    case PHASE_YELLOW:     return YELLOW_DURATION_MS;
  }
  return 0;
}

void TrafficLight::updateNormal(unsigned long now) {
  if (now - phaseStartedAt_ < phaseDuration(phase_)) {
    return;  // current phase still active
  }

  // RED → RED_YELLOW → GREEN → YELLOW → RED
  Phase next;
  switch (phase_) {
    case PHASE_RED:        next = PHASE_RED_YELLOW; break;
    case PHASE_RED_YELLOW: next = PHASE_GREEN;      break;
    case PHASE_GREEN:      next = PHASE_YELLOW;     break;
    case PHASE_YELLOW:     next = PHASE_RED;        break;
    default:               next = PHASE_RED;        break;
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
