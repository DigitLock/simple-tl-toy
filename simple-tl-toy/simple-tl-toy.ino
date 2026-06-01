// simple-tl-toy — HLV Traffic Lights v0.1
// Target: Arduino Nano (ATmega328P), standard Arduino core.
// v0.1 scope: single traffic light TL1 driven by one button (see config.h).

#include "config.h"
#include "traffic_light.h"

// One TrafficLight object per configured light. Default-constructed; pins and
// timings are wired up in setup() via begin(). Scales by bumping
// NUM_TRAFFIC_LIGHTS and adding rows to TRAFFIC_LIGHT_CONFIGS.
TrafficLight trafficLights[NUM_TRAFFIC_LIGHTS];

void setup() {
  // Staggered start: TL i waits i * START_OFFSET_MS before its RED timer
  // begins, so adjacent lights fall a fixed phase-shift out of step.
  for (uint8_t i = 0; i < NUM_TRAFFIC_LIGHTS; ++i) {
    trafficLights[i].begin(TRAFFIC_LIGHT_CONFIGS[i], i * START_OFFSET_MS);
  }
}

void loop() {
  // Single millis() read per iteration keeps all lights on the same clock.
  unsigned long now = millis();
  for (uint8_t i = 0; i < NUM_TRAFFIC_LIGHTS; ++i) {
    trafficLights[i].update(now);
  }
}
