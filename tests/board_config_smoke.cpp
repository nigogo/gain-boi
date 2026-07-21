#include "BoardConfig.hpp"
int main() {
  static_assert(gain_boi::board::kEpaperSck == 12);
  static_assert(gain_boi::board::kHapticSda == 40);
  static_assert(gain_boi::board::kHapticScl == 41);
  static_assert(gain_boi::board::kEncoderA == 6);
  static_assert(gain_boi::board::kEncoderB == 4);
  static_assert(gain_boi::board::kEncoderPress == 5);
  static_assert(gain_boi::board::kMenuButton == 2);
  static_assert(gain_boi::board::kBackButton == 1);
  return 0;
}
