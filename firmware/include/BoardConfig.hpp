#pragma once
#include <cstdint>
namespace gain_boi::board {
// Verified from Elecrow's official 2.13-inch repository README pin definition on 2026-07-21.
// Display: 122x250, SSD1680Z/JD79661-compatible SPI e-paper. Display pins are from the README; controls are from the factory source KEY definitions: HOME=2, EXIT=1, PRV=6, NEXT=4, OK=5.
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
constexpr int kEncoderA = 6;
constexpr int kEncoderB = 4;
constexpr int kEncoderPress = 5;
constexpr int kMenuButton = 2;
constexpr int kBackButton = 1;
constexpr uint32_t kDebounceMs = 25;
static_assert(kDisplayWidth == 122 && kDisplayHeight == 250, "CrowPanel 2.13 resolution changed");
static_assert(kDrv2605Address == 0x5A, "Unexpected DRV2605L address");
}
