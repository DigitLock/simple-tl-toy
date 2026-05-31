// Project-wide configuration: pin assignments, timings, traffic light count.
// To scale up to 4 lights, bump NUM_TRAFFIC_LIGHTS and add entries to
// TRAFFIC_LIGHT_CONFIGS — no other code changes are required.
//
// Target board: Arduino Nano (ATmega328P), 5V logic, standard Arduino core.
// Reserved pins on this build (do not reassign):
//   D0/D1   — UART (USB Serial)
//   D12/D13 — SPI MISO/SCK; on-board nRF24 module is wired here in hardware
//             (driver is not used in this project, but the pins are
//             physically tied to the radio chip — leave them alone)
// Note: D10/D11 (SPI SS/MOSI) were originally reserved for the same nRF24
// module but have been reused for TL4 (Green/Button) since the radio is
// not driven from firmware. Adding nRF24 back would require reassigning TL4.

#pragma once

#include <Arduino.h>

// Number of traffic lights wired to the board.
#define NUM_TRAFFIC_LIGHTS 4

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
  // TL1 — digital pins D2..D5
  { 2, 3, 4, 5, 5000, 5000 },
  // TL2 — digital pins D6..D9
  { 6, 7, 8, 9, 5000, 5000 },
  // TL3 — analog header A0..A3 used as digital I/O
  { A0, A1, A2, A3, 5000, 5000 },
  // TL4 — A4=Red, A5=Yellow, D10=Green, D11=Button.
  // A6/A7 are NOT used: on Nano they are analog-input-only pads with no
  // digital I/O hardware — pinMode/digitalRead/digitalWrite silently no-op,
  // so an LED on A6 will not light and a button on A7 will not respond.
  // NOTE: D10/D11 are the SPI SS/MOSI lines — using them here conflicts
  // with the on-board nRF24 module (see header comment).
  { A4, A5, 10, 11, 5000, 5000 },
  };
