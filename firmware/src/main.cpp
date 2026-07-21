#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include <array>
#include <string>
#include "BoardConfig.hpp"
#include "gain_boi/App.hpp"
using namespace gain_boi;
class EspClock final: public Clock{ public: uint32_t millis() const override { return ::millis(); } };
class PrefStorage final: public Storage{ public: void begin(){ prefs_.begin("gain-boi", false);} std::optional<std::string> load() override{ String s=prefs_.getString("state", ""); if(s.length()==0) return {}; return std::string(s.c_str()); } void save(const std::string& data) override{ prefs_.putString("state", data.c_str()); } private: Preferences prefs_; };
const char* screenName(Screen s){ switch(s){case Screen::Home:return "home";case Screen::StartWorkout:return "start_workout";case Screen::ActiveSet:return "active_set";case Screen::RestTimer:return "rest_timer";case Screen::Summary:return "summary";case Screen::Settings:return "settings";} return "unknown"; }
const char* inputName(Input i){ switch(i){case Input::RotateLeft:return "rotate_left";case Input::RotateRight:return "rotate_right";case Input::EncoderPress:return "encoder_press";case Input::Menu:return "menu";case Input::Back:return "back";} return "unknown"; }
const char* hapticName(HapticPattern p){ switch(p){case HapticPattern::Click:return "click";case HapticPattern::Confirm:return "confirm";case HapticPattern::RestComplete:return "rest_complete";case HapticPattern::Error:return "error";} return "unknown"; }
#ifdef GAIN_BOI_WOKWI
class WokwiHaptics final: public Haptics{ public: void begin(){ Serial.println("GB:HAPTIC:READY adapter=wokwi future_drv2605_addr=0x5A"); } void play(HapticEvent e) override{ Serial.printf("GB:HAPTIC:PLAY effect=%s strength=%u\n", hapticName(e.pattern), e.strength); }};
class WokwiDisplay final: public Display{ public: void render(const ViewModel& v) override{ if(v.title==last_ && v.needs_full_refresh==lastFull_) return; last_=v.title; lastFull_=v.needs_full_refresh; framebuffer_.fill(0); Serial.printf("GB:DISPLAY:REFRESH type=%s screen=%s width=%d height=%d title=\"%s\"\n", v.needs_full_refresh?"full":"partial", screenName(v.screen), board::kDisplayWidth, board::kDisplayHeight, v.title.c_str()); for(size_t i=0;i<v.lines.size();++i) Serial.printf("GB:DISPLAY:LINE index=%u text=\"%s\"\n", static_cast<unsigned>(i), v.lines[i].c_str()); } private: std::string last_; bool lastFull_{false}; std::array<uint8_t, (board::kDisplayWidth*board::kDisplayHeight+7)/8> framebuffer_{}; };
using FirmwareHaptics = WokwiHaptics; using FirmwareDisplay = WokwiDisplay;
#else
class Drv2605Haptics final: public Haptics{ public: void begin(){ Wire.begin(board::kHapticSda, board::kHapticScl); write(0x01,0x00); write(0x1A,0xC0); write(0x1D,0xA8); } void play(HapticEvent e) override{ uint8_t effect=e.pattern==HapticPattern::RestComplete?47:1; write(0x04, effect); write(0x05,0); write(0x0C,1); } private: void write(uint8_t r,uint8_t v){ Wire.beginTransmission(board::kDrv2605Address); Wire.write(r); Wire.write(v); Wire.endTransmission(); }};
class SerialDisplay final: public Display{ public: void render(const ViewModel& v) override{ if(v.title==last_) return; last_=v.title; Serial.println(); Serial.println(v.title.c_str()); for(const auto& l:v.lines) Serial.println(l.c_str()); } private: std::string last_; };
using FirmwareHaptics = Drv2605Haptics; using FirmwareDisplay = SerialDisplay;
#endif
class GpioInputs{ public: void begin(){ for(auto pin:{board::kEncoderA,board::kEncoderB,board::kEncoderPress,board::kMenuButton,board::kBackButton}) if(pin>=0) pinMode(pin, INPUT_PULLUP); lastA_=digitalRead(board::kEncoderA); samplePress(encoderPress_, board::kEncoderPress); samplePress(menu_, board::kMenuButton); samplePress(back_, board::kBackButton); }
  std::optional<Input> poll(){ const auto now=millis(); const int a=digitalRead(board::kEncoderA); if(a!=lastA_ && now-lastEncoderMs_>=board::kDebounceMs){ lastEncoderMs_=now; lastA_=a; if(a==LOW){ const Input in=digitalRead(board::kEncoderB)==HIGH?Input::RotateRight:Input::RotateLeft; log(in); return in; }} if(edge(encoderPress_,board::kEncoderPress,now)){ log(Input::EncoderPress); return Input::EncoderPress;} if(edge(menu_,board::kMenuButton,now)){ log(Input::Menu); return Input::Menu;} if(edge(back_,board::kBackButton,now)){ log(Input::Back); return Input::Back;} return {}; }
 private: struct Button{ int last{HIGH}; uint32_t changed{0}; }; static void samplePress(Button& b,int pin){ if(pin>=0) b.last=digitalRead(pin); } bool edge(Button& b,int pin,uint32_t now){ if(pin<0) return false; int v=digitalRead(pin); if(v!=b.last && now-b.changed>=board::kDebounceMs){ b.changed=now; b.last=v; return v==LOW; } return false;} void log(Input in){ Serial.printf("GB:INPUT event=%s\n", inputName(in)); } int lastA_{HIGH}; uint32_t lastEncoderMs_{0}; Button encoderPress_{},menu_{},back_{}; };
EspClock clock_; PrefStorage storage_; FirmwareHaptics haptics_; FirmwareDisplay display_; GpioInputs inputs_; App app(clock_, storage_, haptics_); Screen lastScreen_=Screen::Home;
void setup(){ Serial.begin(115200); storage_.begin(); haptics_.begin(); inputs_.begin(); app.start(); lastScreen_=app.screen(); const char* target =
#ifdef GAIN_BOI_WOKWI
"wokwi";
#else
"crowpanel";
#endif
 Serial.printf("GB:BOOT ok target=%s\n", target); }
void loop(){ app.tick(); if(auto in=inputs_.poll()) app.handle(*in); if(app.screen()!=lastScreen_){ lastScreen_=app.screen(); Serial.printf("GB:STATE screen=%s\n", screenName(lastScreen_)); } display_.render(app.view()); delay(10); }
