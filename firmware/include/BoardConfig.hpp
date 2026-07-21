#pragma once
#include <cstdint>
namespace gain_boi::board {
// Verified from Elecrow's official 2.13-inch repository README pin definition on 2026-07-21.
// Display: 122x250, SSD1680Z/JD79661-compatible SPI e-paper. Buttons/encoder GPIOs were not published in the README; leave them unset until confirmed from schematic/example code.
constexpr int kDisplayWidth = 122;
constexpr int kDisplayHeight = 250;
constexpr int kEpaperSck = 12;
constexpr int kEpaperMosi = 11;
constexpr int kEpaperReset = 10;
constexpr int kEpaperDc = 13;
constexpr int kEpaperCs = 14;
constexpr int kEpaperBusy = 9;
constexpr int kHapticSda = 40;
constexpr int kHapticScl = 41;
constexpr uint8_t kDrv2605Address = 0x5A;
constexpr int kUnverifiedGpio = -1;
constexpr int kEncoderA = kUnverifiedGpio;
constexpr int kEncoderB = kUnverifiedGpio;
constexpr int kEncoderPress = kUnverifiedGpio;
constexpr int kMenuButton = kUnverifiedGpio;
constexpr int kBackButton = kUnverifiedGpio;
static_assert(kDisplayWidth == 122 && kDisplayHeight == 250, "CrowPanel 2.13 resolution changed");
static_assert(kDrv2605Address == 0x5A, "Unexpected DRV2605L address");
}
