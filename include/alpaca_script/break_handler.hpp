#pragma once

#include <Game/Core.hpp>
#include <alpaca_script/script_state.hpp>
#include <chrono>
#include <random>
#include <thread>

namespace as {
class short_break_settings {
public:
  std::uniform_int_distribution<std::int64_t> interval_distribution;
  std::uniform_int_distribution<std::int64_t> duration_distribution;
  bool lose_focus;

  short_break_settings(auto interval_min, auto interval_max, auto duration_min,
                       auto duration_max, bool lose_focus = true)
      : _generator(std::chrono::system_clock::now().time_since_epoch().count()),
        interval_distribution(
            std::chrono::duration_cast<std::chrono::milliseconds>(interval_min)
                .count(),
            std::chrono::duration_cast<std::chrono::milliseconds>(interval_max)
                .count()),
        duration_distribution(
            std::chrono::duration_cast<std::chrono::milliseconds>(duration_min)
                .count(),
            std::chrono::duration_cast<std::chrono::milliseconds>(duration_max)
                .count()),
        lose_focus(lose_focus) {}

  std::int64_t interval() { return interval_distribution(_generator); }

  std::int64_t duration() { return duration_distribution(_generator); }

private:
  std::default_random_engine _generator;
};

class break_handler {
public:
  short_break_settings sbs;

  break_handler(short_break_settings sbs)
      : sbs(sbs), _next_short_break(std::chrono::system_clock::now() +
                                    std::chrono::milliseconds(sbs.interval())) {
  }

  bool should_short_break() const {
    auto now = std::chrono::system_clock::now();
    return now >= _next_short_break;
  }

  void short_break() {
    Antiban::MouseOffClient(true);
    auto duration = std::chrono::milliseconds(sbs.duration());
    std::cout << "Short breaking for " << duration.count() << "ms" << std::endl;
    std::this_thread::sleep_for(duration);
    _next_short_break += std::chrono::milliseconds(sbs.interval());
  }

private:
  std::chrono::system_clock::time_point _next_short_break;
};

} // namespace as