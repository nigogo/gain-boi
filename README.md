# Gain Boi

Gain Boi is a portable C++17 gym companion that runs the same workout state machine in a desktop simulator and on an Elecrow CrowPanel ESP32-S3 2.13-inch e-paper HMI.

## Hardware and wiring

- Elecrow CrowPanel ESP32 2.13-inch E-Paper HMI: ESP32-S3, 122×250 monochrome partial-refresh e-paper, rotary encoder, Menu and Back buttons, USB-C serial/programming.
- Pimoroni PIM452 DRV2605L haptic breakout with integrated LRA:
  - SDA: GPIO40
  - SCL: GPIO41
  - VCC: 3.3 V
  - GND: GND
  - I²C address: `0x5A`

Board-specific values live in `firmware/include/BoardConfig.hpp`. The display SPI pins are taken from Elecrow's official 2.13-inch repository README: SCK 12, MOSI 11, RES 10, DC 13, CS 14, BUSY 9. The current README confirms the display controller family, resolution, and partial refresh support, but it does not publish GPIO numbers for the rotary encoder/Menu/Back controls; those are intentionally left as `-1` until confirmed from official examples or schematic.

## Architecture

- `gain_boi_core`: Arduino-free shared library containing the app state machine, screen view models, workout/set/rep state, rest timer, settings, persistence serialization, haptic requests, and small hardware interfaces (`Display`, `Clock`, `Storage`, `Haptics`).
- `simulator`: CMake-built desktop console simulator. It renders a 122×250-style monochrome textual representation, logs haptics, uses a local state file, and uses a monotonic clock. It avoids platform-specific GUI assumptions so command-line builds stay reproducible.
- `firmware`: PlatformIO Arduino-ESP32 adapter layer. `setup()` initializes storage, I²C haptics, and the app; `loop()` ticks the app and renders the current view. The DRV2605L is initialized over `Wire.begin(40, 41)` and configured for LRA mode.

No business logic is in display, GPIO, or Arduino-specific code.

## Repository structure

```text
core/include/gain_boi/  Shared public interfaces and app API
core/src/               Shared implementation
simulator/src/          Desktop adapters and simulator entry point
firmware/include/       Central board configuration
firmware/src/           ESP32 adapters and Arduino entry point
tests/                  CTest/doctest shared-core tests
```

## Required tools

- CMake 3.22+
- A C++17 compiler
- PlatformIO CLI
- Network access for first-time dependency fetches (`doctest` for CMake, pinned pioarduino platform for firmware)

## Desktop build, test, and run

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
./build/simulator/gain_boi_sim
```

Optional coverage helper:

```bash
cmake -S . -B build-coverage -DGAIN_BOI_COVERAGE=ON
cmake --build build-coverage
ctest --test-dir build-coverage
cmake --build build-coverage --target coverage
```

## Simulator controls

- `a`: rotate encoder left
- `d`: rotate encoder right
- `Enter`: encoder press
- `m`: Menu
- `b`, `q`, or Escape: Back
- `x`: exit simulator

## Firmware build and device commands

```bash
pio run -e crowpanel
pio run -e crowpanel -t upload
pio device monitor
```

The upload and monitor commands require a connected CrowPanel. Do not treat compilation as proof of physical display/input behavior.

## Adding screens, states, or adapters

1. Add any new state or view data to `core/include/gain_boi/App.hpp`.
2. Implement state transitions and view-model text in `core/src/App.cpp`.
3. Add tests in `tests/core_tests.cpp` before wiring hardware-specific behavior.
4. Implement adapter-only rendering/input/storage details in `simulator/src` or `firmware/src` without moving business rules out of the core.
5. Add board constants only to `firmware/include/BoardConfig.hpp`, with a source note for any value copied from vendor documentation.

## Still requiring real-device testing

- Exact CrowPanel rotary encoder, Menu, and Back GPIO mappings.
- E-paper controller initialization and partial-refresh waveforms on the physical 2.13-inch module.
- DRV2605L LRA strength/timing with the PIM452 integrated actuator.
- Button debounce and encoder direction on real hardware.

## Verified limitations

Elecrow's current public README for this exact 2.13-inch repository lists the display pin definitions and display capabilities, but not the rotary encoder/Menu/Back GPIO mappings. This project therefore compiles a smoke-checked board configuration while avoiding invented input pin assignments.
