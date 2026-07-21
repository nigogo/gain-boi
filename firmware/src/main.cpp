#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include "BoardConfig.hpp"
#include "gain_boi/App.hpp"
using namespace gain_boi;
class EspClock final: public Clock{ public: uint32_t millis() const override { return ::millis(); } };
class PrefStorage final: public Storage{ public: void begin(){ prefs_.begin("gain-boi", false);} std::optional<std::string> load() override{ String s=prefs_.getString("state", ""); if(s.length()==0) return {}; return std::string(s.c_str()); } void save(const std::string& data) override{ prefs_.putString("state", data.c_str()); } private: Preferences prefs_; };
class Drv2605Haptics final: public Haptics{ public: void begin(){ Wire.begin(board::kHapticSda, board::kHapticScl); write(0x01,0x00); write(0x1A,0xC0); write(0x1D,0xA8); } void play(HapticEvent e) override{ uint8_t effect=e.pattern==HapticPattern::RestComplete?47:1; write(0x04, effect); write(0x05,0); write(0x0C,1); } private: void write(uint8_t r,uint8_t v){ Wire.beginTransmission(board::kDrv2605Address); Wire.write(r); Wire.write(v); Wire.endTransmission(); }};
class SerialDisplay final: public Display{ public: void render(const ViewModel& v) override{ if(v.title==last_) return; last_=v.title; Serial.println(); Serial.println(v.title.c_str()); for(const auto& l:v.lines) Serial.println(l.c_str()); } private: std::string last_; };
EspClock clock_; PrefStorage storage_; Drv2605Haptics haptics_; SerialDisplay display_; App app(clock_, storage_, haptics_);
void setup(){ Serial.begin(115200); storage_.begin(); haptics_.begin(); app.start(); Serial.println("Gain Boi firmware booted; display GPIO config compiled."); }
void loop(){ app.tick(); display_.render(app.view()); delay(50); }
