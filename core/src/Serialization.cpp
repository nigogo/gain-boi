#include "gain_boi/App.hpp"
#include <cstdio>
namespace gain_boi {
std::string serialize(const Settings& s, const WorkoutSession& w){ return "v1,"+std::to_string(s.rest_seconds)+","+(s.haptics_enabled?"1":"0")+","+std::to_string(w.set)+","+std::to_string(w.reps)+","+std::to_string(w.total_reps)+","+(w.active?"1":"0"); }
bool deserialize(const std::string& text, Settings& s, WorkoutSession& w){ unsigned rest,hap,set,reps,total,active; if(std::sscanf(text.c_str(),"v1,%u,%u,%u,%u,%u,%u",&rest,&hap,&set,&reps,&total,&active)!=6) return false; if(rest<15||rest>600||reps>99) return false; s.rest_seconds=rest; s.haptics_enabled=hap!=0; w.set=set; w.reps=reps; w.total_reps=total; w.active=active!=0; return true; }
}
