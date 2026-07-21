#include "gain_boi/App.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
using namespace gain_boi;
class DesktopClock: public Clock{ public: uint32_t millis() const override{ using namespace std::chrono; return static_cast<uint32_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count()); }};
class FileStorage: public Storage{ public: std::optional<std::string> load() override{ std::ifstream f("gain_boi_state.txt"); if(!f) return {}; return std::string((std::istreambuf_iterator<char>(f)),{});} void save(const std::string& s) override{ std::ofstream("gain_boi_state.txt")<<s; }};
class ConsoleHaptics: public Haptics{ public: void play(HapticEvent e) override{ std::cout<<"[haptic "<<static_cast<int>(e.strength)<<"]\n"; }};
class MonoDisplay: public Display{ public: void render(const ViewModel& v) override{ std::cout<<"\033[2J\033[H"; std::cout<<"+ Gain Boi 122x250 simulator +\n"<<v.title<<"\n"; for(const auto& l:v.lines) std::cout<<"  "<<l<<"\n"; std::cout<<"\nControls: a/d rotate, enter press, m menu, b/q back, x exit\n"; }};
int main(){ DesktopClock c; FileStorage s; ConsoleHaptics h; MonoDisplay d; App app(c,s,h); app.start(); char ch='\0'; while(true){ app.tick(); d.render(app.view()); ch=static_cast<char>(std::cin.get()); if(ch=='x') break; if(ch=='a') app.handle(Input::RotateLeft); else if(ch=='d') app.handle(Input::RotateRight); else if(ch=='\n') app.handle(Input::EncoderPress); else if(ch=='m'||ch=='M') app.handle(Input::Menu); else if(ch=='b'||ch=='q'||ch==27) app.handle(Input::Back); } }
