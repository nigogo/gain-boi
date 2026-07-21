# Gain Boi Wokwi target

The Wokwi target builds the existing Arduino firmware composition with mockable firmware adapters selected by `GAIN_BOI_WOKWI`.

## Diagram

`../diagram.json` uses an ESP32-S3 DevKitC-compatible board, a rotary encoder, Menu and Back buttons, serial output, and a logic analyzer. The controls share the same logical board configuration as the physical CrowPanel firmware:

- Encoder A / previous: GPIO6
- Encoder B / next: GPIO4
- Encoder press / OK: GPIO5
- Menu / Home: GPIO2
- Back / Exit: GPIO1

These values come from Elecrow's factory source key definitions for the CrowPanel 2.13-inch e-paper HMI.

## Scenario

`scenarios/workout-smoke.yaml` documents the intended smoke path and stable serial markers for automation-capable Wokwi runners: boot, home, start workout, active set, complete set, rest timer, and back/continue behavior. If your installed `wokwi-cli` does not support YAML scenarios directly, run `wokwi-cli .` and drive the controls manually while checking the same `GB:*` serial markers.

## Extension points

- A custom DRV2605L I²C model at address `0x5A` could be added later under `wokwi/chips/` if it is implemented cleanly.
- The Wokwi display adapter intentionally models only a 122×250 monochrome framebuffer and serial refresh markers; it does not emulate e-paper ghosting, refresh timing, or panel physics.
