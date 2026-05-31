// Single traffic light driver: phase state machine + debounced mode-switch button.
// Non-blocking — update() must be called every loop() iteration with the current
// millis() timestamp.

#pragma once

#include <Arduino.h>
#include "config.h"

class TrafficLight {
 public:
  TrafficLight();

  // Configure pins and enter normal mode starting from RED.
  void begin(const TrafficLightConfig& cfg);

  // Advance the state machine. `now` should be the result of millis().
  void update(unsigned long now);

 private:
  enum Phase : uint8_t {
    PHASE_RED,
    PHASE_RED_YELLOW,
    PHASE_GREEN,
    PHASE_YELLOW
  };

  enum Mode : uint8_t {
    MODE_NORMAL,
    MODE_EMERGENCY
  };

  TrafficLightConfig cfg_;

  // Phase / mode state
  Phase         phase_;
  Mode          mode_;
  unsigned long phaseStartedAt_;

  // Emergency-mode blink state
  bool          blinkOn_;
  unsigned long lastBlinkAt_;

  // Button debounce state (INPUT_PULLUP: HIGH = released, LOW = pressed)
  int           lastButtonReading_;
  int           stableButtonState_;
  unsigned long lastDebounceAt_;

  // LED helpers
  void setLights(bool r, bool y, bool g);

  // State transitions
  void enterPhase(Phase p, unsigned long now);
  void enterNormalMode(unsigned long now);
  void enterEmergencyMode(unsigned long now);

  // Per-mode tick handlers
  void updateNormal(unsigned long now);
  void updateEmergency(unsigned long now);

  // How long the given phase should last (uses per-light config + shared timings).
  unsigned long phaseDuration(Phase p) const;

  // Returns true exactly once per debounced button press (release→press edge).
  bool pollButtonPressed(unsigned long now);
};
