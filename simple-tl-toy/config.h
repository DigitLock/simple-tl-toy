// Project-wide configuration: pin assignments, timings, traffic light count.
// To scale up to 4 lights, bump NUM_TRAFFIC_LIGHTS and add entries to
// TRAFFIC_LIGHT_CONFIGS — no other code changes are required.
//
// Target board: Arduino Nano (ATmega328P), 5V logic, standard Arduino core.
// Reserved pins on this build (do not reassign):
//   D0/D1   — UART (USB Serial)
//   D10-D13 — SPI, occupied by on-board nRF24 module

#pragma once

#include <Arduino.h>

// Number of traffic lights wired to the board. v0.1 = 1 (TL1 only).
#define NUM_TRAFFIC_LIGHTS 1

// Shared timings (milliseconds) applied to every traffic light.
constexpr unsigned long YELLOW_DURATION_MS         = 1500;  // solid yellow phase
constexpr unsigned long RED_YELLOW_DURATION_MS     = 1500;  // red+yellow "get ready"
constexpr unsigned long EMERGENCY_BLINK_INTERVAL_MS = 500;  // half-period of yellow blink
constexpr unsigned long DEBOUNCE_DELAY_MS          = 50;    // button debounce window

// Per-light configuration: pins + individual red/green durations.
struct TrafficLightConfig {
  uint8_t       pinRed;
  uint8_t       pinYellow;
  uint8_t       pinGreen;
  uint8_t       pinButton;
  unsigned long redDurationMs;
  unsigned long greenDurationMs;
};

// Table of all traffic lights. Index corresponds to TL number (0 = TL1).
const TrafficLightConfig TRAFFIC_LIGHT_CONFIGS[NUM_TRAFFIC_LIGHTS] = {
  // TL1
  { 2, 3, 4, 5, 5000, 5000 },

  // Future entries (uncomment + bump NUM_TRAFFIC_LIGHTS to enable):
  // TL2 — digital pins D6..D9
  // { 6, 7, 8, 9, 5000, 5000 },
  // TL3 — analog header A0..A3 used as digital I/O
  // { A0, A1, A2, A3, 5000, 5000 },
  // TL4 — WARNING: A6 and A7 on Nano are analog-input only; digitalRead/Write
  // do NOT work on them. To enable TL4, reroute its Yellow/Button to free
  // digital pins or switch to a board where A6/A7 are GPIO-capable.
  // { A4, A5, A6, A7, 5000, 5000 },
};
