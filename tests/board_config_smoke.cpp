#include "BoardConfig.hpp"
int main() {
  static_assert(gain_boi::board::kEpaperSck == 12);
  static_assert(gain_boi::board::kHapticSda == 40);
  static_assert(gain_boi::board::kHapticScl == 41);
  static_assert(gain_boi::board::kEncoderA == gain_boi::board::kUnverifiedGpio);
  return 0;
}
