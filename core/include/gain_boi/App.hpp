#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
namespace gain_boi {
enum class Input { RotateLeft, RotateRight, EncoderPress, Menu, Back };
enum class Screen { Home, StartWorkout, ActiveSet, RestTimer, Summary, Settings };
enum class HapticPattern { Click, Confirm, RestComplete, Error };
struct HapticEvent { HapticPattern pattern; uint8_t strength; };
struct Settings { uint32_t rest_seconds{60}; bool haptics_enabled{true}; };
struct WorkoutSession { uint16_t set{0}; uint16_t reps{0}; uint16_t total_reps{0}; bool active{false}; };
struct ViewModel { Screen screen; std::string title; std::vector<std::string> lines; bool needs_full_refresh{false}; };
class Clock { public: virtual ~Clock() = default; virtual uint32_t millis() const = 0; };
class Display { public: virtual ~Display() = default; virtual void render(const ViewModel&) = 0; };
class Storage { public: virtual ~Storage() = default; virtual std::optional<std::string> load() = 0; virtual void save(const std::string&) = 0; };
class Haptics { public: virtual ~Haptics() = default; virtual void play(HapticEvent) = 0; };
class App {
 public:
  App(Clock& clock, Storage& storage, Haptics& haptics);
  void start(); void tick(); void handle(Input input); ViewModel view() const;
  Screen screen() const { return screen_; } const WorkoutSession& session() const { return session_; }
  const Settings& settings() const { return settings_; } std::vector<HapticEvent> drainHapticsForTests();
 private:
  void emit(HapticPattern pattern); void persist(); void enterRest(); void finishWorkout();
  Clock& clock_; Storage& storage_; Haptics& haptics_; Settings settings_{}; WorkoutSession session_{};
  Screen screen_{Screen::Home}; uint32_t rest_started_ms_{0}; bool started_{false}; std::vector<HapticEvent> haptic_log_;
};
std::string serialize(const Settings&, const WorkoutSession&);
bool deserialize(const std::string&, Settings&, WorkoutSession&);
}
