// Single traffic light driver: phase state machine + debounced mode-switch button.
// Non-blocking — update() must be called every loop() iteration with the current
// millis() timestamp.

#pragma once

#include <Arduino.h>
#include "config.h"

class TrafficLight {
 public:
  TrafficLight();

  // Configure pins and either start the normal cycle immediately
  // (startDelayMs == 0) or hold on solid RED for startDelayMs before the
  // first RED phase timer begins (used to stagger multiple lights at boot).
  void begin(const TrafficLightConfig& cfg, unsigned long startDelayMs = 0);

  // Advance the state machine. `now` should be the result of millis().
  void update(unsigned long now);

 private:
  enum Phase : uint8_t {
    PHASE_RED,
    PHASE_RED_YELLOW,
    PHASE_GREEN,
    PHASE_GREEN_BLINK,  // green blinks N times before YELLOW
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

  // Staggered-start state: while waitingToStart_ is true, the light shows
  // solid RED and the FSM is held until millis() >= startAt_.
  bool          waitingToStart_;
  unsigned long startAt_;

  // GREEN_BLINK sub-state: tracks the off/on toggling and how many full
  // on-periods have already elapsed inside the phase.
  bool          greenBlinkOn_;
  uint8_t       greenBlinkOnsDone_;
  unsigned long greenBlinkLastToggleAt_;

  // LED helpers
  void setLights(bool r, bool y, bool g);

  // State transitions
  void enterPhase(Phase p, unsigned long now);
  void enterNormalMode(unsigned long now);
  void enterEmergencyMode(unsigned long now);

  // Per-mode tick handlers
  void updateNormal(unsigned long now);
  void updateEmergency(unsigned long now);
  void updateGreenBlink(unsigned long now);

  // How long the given phase should last (uses per-light config + shared
  // timings). PHASE_GREEN_BLINK self-manages its timing and returns 0 here.
  unsigned long phaseDuration(Phase p) const;

  // Returns true exactly once per debounced button press (release→press edge).
  bool pollButtonPressed(unsigned long now);
};
