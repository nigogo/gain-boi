# Gain Boi

Gain Boi is a portable C++17 gym companion with one Arduino-free workout state machine and three execution targets:

- **Desktop simulator**: fast application logic and UI development through CMake/CTest.
- **Wokwi**: ESP32-S3 firmware, GPIO wiring, input event, display-refresh, and haptic-request simulation.
- **CrowPanel device**: real Elecrow CrowPanel ESP32-S3 2.13-inch e-paper HMI validation.

## Hardware and wiring

- Elecrow CrowPanel ESP32 2.13-inch E-Paper HMI: ESP32-S3, 8 MB flash, 8 MB PSRAM, 122×250 monochrome partial-refresh e-paper, rotary encoder, Menu/Home and Back/Exit buttons, USB-C serial/programming.
- Pimoroni PIM452 DRV2605L haptic breakout with integrated LRA:
  - SDA: GPIO40
  - SCL: GPIO41
  - VCC: 3.3 V
  - GND: GND
  - I²C address: `0x5A`

Board-specific values live in `firmware/include/BoardConfig.hpp`. Display SPI pins are from Elecrow's official 2.13-inch repository README: SCK 12, MOSI 11, RES 10, DC 13, CS 14, BUSY 9. Control pins are from Elecrow's factory source key definitions: Menu/Home GPIO2, Back/Exit GPIO1, encoder previous/A GPIO6, encoder next/B GPIO4, and encoder press/OK GPIO5.

## Architecture

- `gain_boi_core`: Arduino-free shared library containing the app state machine, screen view models, workout/set/rep state, rest timer, settings, persistence serialization, haptic requests, and small hardware interfaces (`Display`, `Clock`, `Storage`, `Haptics`).
- `simulator`: CMake-built desktop console simulator. It renders a 122×250-style monochrome textual representation, logs haptics, uses a local state file, and uses a monotonic clock.
- `firmware`: PlatformIO Arduino-ESP32 adapter layer. The physical `crowpanel` environment uses the real DRV2605L-over-I²C haptic adapter and keeps the existing serial display adapter. The `crowpanel-wokwi` environment reuses the same app and GPIO input composition with Wokwi display and haptic adapters selected by `GAIN_BOI_WOKWI`.

No business logic is in display, GPIO, Wokwi, or Arduino-specific code.

## Repository structure

```text
core/include/gain_boi/      Shared public interfaces and app API
core/src/                   Shared implementation
simulator/src/              Desktop adapters and simulator entry point
firmware/include/           Central board configuration
firmware/src/               ESP32 adapters and Arduino entry point
tests/                      CTest shared-core and board-config smoke tests
wokwi/                      Wokwi notes and automation scenario
diagram.json                Wokwi virtual wiring
wokwi.toml                  Wokwi firmware/ELF paths
```

## Required tools

- CMake 3.22+
- A C++17 compiler
- PlatformIO CLI
- Wokwi CLI for simulation (`wokwi-cli`)
- Network access for first-time dependency fetches (`doctest` for CMake, pinned pioarduino platform for firmware)

## Desktop build, test, and run

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
./build/simulator/gain_boi_sim
```

Simulator controls: `a` rotate left, `d` rotate right, `Enter` press, `m` Menu, `b`/`q`/Escape Back, `x` exit.

## Firmware build and device commands

```bash
pio run -e crowpanel
pio run -e crowpanel -t upload
pio device monitor
```

Upload and monitor require a connected CrowPanel. Compilation is not proof of physical display/input/haptic behavior.

## Wokwi build and run

Build the ESP32-S3 Wokwi firmware:

```bash
pio run -e crowpanel-wokwi
```

Run the simulator from the repository root after creating a Wokwi account/token if your CLI requires one:

```bash
export WOKWI_CLI_TOKEN=... # or follow your installed CLI's token setup; do not commit secrets
wokwi-cli .
```

`wokwi.toml` points Wokwi at `.pio/build/crowpanel-wokwi/firmware.bin` and `.pio/build/crowpanel-wokwi/firmware.elf`.

### Virtual controls

- Rotary encoder clockwise / counterclockwise: GPIO6 and GPIO4 quadrature lines.
- Encoder press: GPIO5.
- Menu button: GPIO2.
- Back button: GPIO1.
- Buttons use pull-ups in firmware and bounce is enabled in the Wokwi diagram where supported.

### What Wokwi validates

Wokwi validates ESP32-S3 firmware startup, GPIO wiring, normalized input serial markers (`GB:INPUT`), app state-transition serial markers (`GB:STATE`), Wokwi display refresh markers (`GB:DISPLAY:REFRESH`), and Wokwi haptic request markers (`GB:HAPTIC:PLAY`). The display adapter maintains a 122×250 monochrome framebuffer model and reports full versus partial refresh requests over serial.

### What still requires physical hardware

Wokwi does **not** validate:

- E-paper ghosting or physical refresh timing
- Actual haptic feel
- Battery consumption
- Charging behavior
- Deep-sleep current
- Electrical signal integrity

Physical CrowPanel testing is still required for real e-paper controller initialization/waveforms, encoder direction and debounce feel, DRV2605L actuator strength/timing, power behavior, charging behavior, and any electrical validation.

## CLion integration

Keep CMake as the primary CLion project model by opening the repository CMake project. Add optional CLion external tools or run configurations for these commands without committing IDE-generated files:

```bash
pio run -e crowpanel-wokwi
wokwi-cli .
```

## Adding screens, states, or adapters

1. Add any new state or view data to `core/include/gain_boi/App.hpp`.
2. Implement state transitions and view-model text in `core/src/App.cpp`.
3. Add tests in `tests/core_tests.cpp` before wiring hardware-specific behavior.
4. Implement adapter-only rendering/input/storage details in `simulator/src` or `firmware/src` without moving business rules out of the core.
5. Add board constants only to `firmware/include/BoardConfig.hpp`, with a source note for any value copied from vendor documentation.
