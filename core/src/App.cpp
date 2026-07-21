#include "gain_boi/App.hpp"
#include <algorithm>
namespace gain_boi {
App::App(Clock& c, Storage& s, Haptics& h): clock_(c), storage_(s), haptics_(h) {}
void App::start(){ if(auto data=storage_.load()){ Settings st; WorkoutSession ws; if(deserialize(*data, st, ws)){ settings_=st; session_=ws; }} started_=true; }
void App::emit(HapticPattern p){ if(!settings_.haptics_enabled) return; HapticEvent e{p, static_cast<uint8_t>(p==HapticPattern::Error?80:120)}; haptics_.play(e); haptic_log_.push_back(e); }
void App::persist(){ storage_.save(serialize(settings_, session_)); }
void App::handle(Input in){ if(!started_) start();
  if(in==Input::Menu){ screen_=Screen::Settings; emit(HapticPattern::Click); return; }
  if(in==Input::Back){ if(screen_==Screen::Home){ emit(HapticPattern::Error); return; } screen_=Screen::Home; emit(HapticPattern::Click); return; }
  switch(screen_){
    case Screen::Home: if(in==Input::EncoderPress){ screen_=Screen::StartWorkout; emit(HapticPattern::Click);} else emit(HapticPattern::Error); break;
    case Screen::StartWorkout: if(in==Input::EncoderPress){ session_={1,0,0,true}; screen_=Screen::ActiveSet; persist(); emit(HapticPattern::Confirm);} else emit(HapticPattern::Error); break;
    case Screen::ActiveSet: if(in==Input::RotateRight){ session_.reps=std::min<uint16_t>(99, session_.reps+1); emit(HapticPattern::Click);} else if(in==Input::RotateLeft){ if(session_.reps==0) emit(HapticPattern::Error); else {--session_.reps; emit(HapticPattern::Click);} } else if(in==Input::EncoderPress){ enterRest(); } else emit(HapticPattern::Error); persist(); break;
    case Screen::RestTimer: if(in==Input::EncoderPress){ ++session_.set; session_.reps=0; screen_=Screen::ActiveSet; emit(HapticPattern::Confirm); persist(); } else if(in==Input::RotateRight){ finishWorkout(); } else emit(HapticPattern::Error); break;
    case Screen::Summary: if(in==Input::EncoderPress){ session_={}; screen_=Screen::Home; persist(); emit(HapticPattern::Confirm);} else emit(HapticPattern::Error); break;
    case Screen::Settings: if(in==Input::RotateRight){ settings_.rest_seconds=std::min<uint32_t>(600, settings_.rest_seconds+15); persist(); emit(HapticPattern::Click);} else if(in==Input::RotateLeft){ settings_.rest_seconds=settings_.rest_seconds>15?settings_.rest_seconds-15:15; persist(); emit(HapticPattern::Click);} else if(in==Input::EncoderPress){ screen_=Screen::Home; emit(HapticPattern::Confirm);} else emit(HapticPattern::Error); break;
  }}
void App::enterRest(){ session_.total_reps=static_cast<uint16_t>(session_.total_reps+session_.reps); rest_started_ms_=clock_.millis(); screen_=Screen::RestTimer; emit(HapticPattern::Confirm); persist(); }
void App::finishWorkout(){ session_.active=false; screen_=Screen::Summary; emit(HapticPattern::Confirm); persist(); }
void App::tick(){ if(!started_) start(); if(screen_==Screen::RestTimer && clock_.millis()-rest_started_ms_>=settings_.rest_seconds*1000U){ emit(HapticPattern::RestComplete); ++session_.set; session_.reps=0; screen_=Screen::ActiveSet; persist(); }}
ViewModel App::view() const{ ViewModel v{screen_,{}, {}, screen_==Screen::Home}; switch(screen_){case Screen::Home:v.title="Gain Boi";v.lines={"Press Enter to start","M settings"};break;case Screen::StartWorkout:v.title="Start workout";v.lines={"Press Enter confirm","Back cancels"};break;case Screen::ActiveSet:v.title="Set "+std::to_string(session_.set);v.lines={"Reps: "+std::to_string(session_.reps),"Left/Right adjust","Enter starts rest"};break;case Screen::RestTimer:{auto elapsed=(clock_.millis()-rest_started_ms_)/1000U; auto remain= elapsed>=settings_.rest_seconds?0:settings_.rest_seconds-elapsed;v.title="Rest";v.lines={std::to_string(remain)+"s left","Enter skip","Right finish"};break;}case Screen::Summary:v.title="Summary";v.lines={"Total reps: "+std::to_string(session_.total_reps),"Enter done"};break;case Screen::Settings:v.title="Settings";v.lines={"Rest: "+std::to_string(settings_.rest_seconds)+"s","Left/Right adjust"};break;} return v; }
std::vector<HapticEvent> App::drainHapticsForTests(){ auto out=haptic_log_; haptic_log_.clear(); return out; }
}
