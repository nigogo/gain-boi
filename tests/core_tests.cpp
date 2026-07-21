#include "gain_boi/App.hpp"
#include <iostream>
#include <stdexcept>
using namespace gain_boi;
#define CHECK(x) do { if(!(x)) throw std::runtime_error(#x); } while(false)
#define RUN(name) do { try { name(); std::cout << #name << " ok\n"; } catch(const std::exception& e) { std::cerr << #name << " failed: " << e.what() << "\n"; return 1; } } while(false)
struct C:Clock{uint32_t now=0; uint32_t millis() const override{return now;}}; struct S:Storage{std::optional<std::string> data; std::optional<std::string> load() override{return data;} void save(const std::string& s) override{data=s;}}; struct H:Haptics{std::vector<HapticEvent> es; void play(HapticEvent e) override{es.push_back(e);}};
void state_transitions() { C c; S s; H h; App a(c,s,h); a.start(); CHECK(a.screen()==Screen::Home); a.handle(Input::EncoderPress); CHECK(a.screen()==Screen::StartWorkout); a.handle(Input::EncoderPress); CHECK(a.screen()==Screen::ActiveSet); }
void rotary_and_invalid_input() { C c; S s; H h; App a(c,s,h); a.start(); a.handle(Input::EncoderPress); a.handle(Input::EncoderPress); a.handle(Input::RotateLeft); CHECK(h.es.back().pattern==HapticPattern::Error); a.handle(Input::RotateRight); CHECK(a.session().reps==1); }
void rest_timer_completes() { C c; S s; H h; App a(c,s,h); a.start(); a.handle(Input::EncoderPress); a.handle(Input::EncoderPress); a.handle(Input::RotateRight); a.handle(Input::EncoderPress); CHECK(a.screen()==Screen::RestTimer); c.now=60000; a.tick(); CHECK(a.screen()==Screen::ActiveSet); CHECK(a.session().set==2); CHECK(h.es.back().pattern==HapticPattern::RestComplete); }
void navigation_and_settings() { C c; S s; H h; App a(c,s,h); a.start(); a.handle(Input::Menu); CHECK(a.screen()==Screen::Settings); a.handle(Input::RotateRight); CHECK(a.settings().rest_seconds==75); a.handle(Input::Back); CHECK(a.screen()==Screen::Home); }
void serialization_mapping() { Settings st; WorkoutSession ws; CHECK(!deserialize("junk", st, ws)); CHECK(!deserialize("v1,1,1,1,1,1,1", st, ws)); CHECK(deserialize("v1,60,1,2,3,4,1", st, ws)); CHECK(ws.total_reps==4); CHECK(serialize(st, ws).find("v1,")==0); }
int main(){ RUN(state_transitions); RUN(rotary_and_invalid_input); RUN(rest_timer_completes); RUN(navigation_and_settings); RUN(serialization_mapping); }
