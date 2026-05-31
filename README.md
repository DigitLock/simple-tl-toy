# Simple Traffic Light Toy

Toy traffic light system for a kid's road puzzle, powered by STM32F411 (Black Pill).

## Hardware

- **MCU:** STM32F411CEU6 (WeAct Black Pill v3.0)
- **Traffic lights:** 4× LED Traffic Light Module V1224 (5mm LED, common cathode)
- **Buttons:** 4× tactile buttons (one per traffic light)
- **Power:** 18650 + MT3608 boost converter → 5V
- **IDE:** Arduino IDE + STM32duino

## Features (v1.0)

- 4 independent traffic lights with standard R→R+Y→G→Y cycle
- Individual red/green timings per traffic light
- Per-light button toggles between normal and emergency (blinking yellow) mode
- Non-blocking architecture (`millis()` based)

## Documentation

- [Technical Specification (RU)](docs/HLV_TrafficLights_TZ.md)

## Project: Home Lab Vibes

YouTube: [Home Lab Vibes](https://www.youtube.com/@HomeLabVibes)
