#pragma once

#include <chrono>
#include <cmath>
#include <random>
#include <thread>

namespace as {
class break_settings {
public:
  std::normal_distribution<> interval_distribution;
  std::normal_distribution<> duration_distribution;

  template <typename IntervalDuration, typename DurationDuration>
  break_settings(IntervalDuration interval_mean, double interval_deviation,
                 DurationDuration duration_mean, double duration_deviation) {
    const auto interval_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(interval_mean).count();
    const auto duration_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(duration_mean).count();

    interval_distribution = std::normal_distribution<>(
        interval_seconds, interval_seconds * interval_deviation);

    duration_distribution = std::normal_distribution<>(
        duration_seconds, duration_seconds * duration_deviation);

    _random_device.seed(
        std::chrono::system_clock::now().time_since_epoch().count());
  }

  template <typename Duration = std::chrono::seconds> Duration interval() {
    return std::chrono::duration_cast<Duration>(
        std::chrono::duration<double>(interval_distribution(_random_device)));
  }

  template <typename Duration = std::chrono::seconds> Duration duration() {
    return std::chrono::duration_cast<Duration>(
        std::chrono::duration<double>(duration_distribution(_random_device)));
  }

private:
  std::default_random_engine _random_device;
};

class break_handler {
public:
  using clock = std::chrono::steady_clock;

  break_settings tiny_break_settings = break_settings(
      std::chrono::seconds(30), 0.40, std::chrono::seconds(4), 0.40);

  break_settings mini_break_settings = break_settings(
      std::chrono::minutes(10), 0.40, std::chrono::minutes(2), 0.50);

  break_settings short_break_settings = break_settings(
      std::chrono::minutes(30), 0.40, std::chrono::minutes(5), 0.50);

  break_settings long_break_settings =
      break_settings(std::chrono::hours(6), 0.20, std::chrono::hours(2), 0.50);

  clock::time_point next_tiny_break;
  clock::time_point next_mini_break;
  clock::time_point next_short_break;
  clock::time_point next_long_break;

  break_handler() {
    const auto now = clock::now();
    next_tiny_break = now + tiny_break_settings.interval();
    next_mini_break = now + mini_break_settings.interval();
    next_short_break = now + short_break_settings.interval();
    next_long_break = now + long_break_settings.interval();
  }

  break_handler(const break_settings &tiny_break_settings,
                const break_settings &mini_break_settings,
                const break_settings &short_break_settings,
                const break_settings &long_break_settings)
      : tiny_break_settings(tiny_break_settings),
        mini_break_settings(mini_break_settings),
        short_break_settings(short_break_settings),
        long_break_settings(long_break_settings) {}

  bool should_tiny_break() { return clock::now() > next_tiny_break; }

  bool should_mini_break() { return clock::now() > next_mini_break; }

  bool should_short_break() { return clock::now() > next_short_break; }

  bool should_long_break() { return clock::now() > next_long_break; }

  bool should_break() {
    return should_tiny_break() || should_mini_break() || should_short_break() ||
           should_long_break();
  }

  bool tiny_break(const auto &predicate) {
    if (!should_tiny_break())
      return false;

    const auto duration = tiny_break_settings.duration<std::chrono::seconds>();
    Debug::Info << "Taking a tiny break for " << duration.count() << " seconds"
                << std::endl;

    const auto end = clock::now() + duration;
    while (end > clock::now()) {
      if (predicate())
        break;

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    const auto interval = tiny_break_settings.interval<std::chrono::seconds>();
    next_tiny_break = clock::now() + interval;
    Debug::Info << "Next tiny break in " << interval.count() << " seconds"
                << std::endl;
    return true;
  }

  bool tiny_break() { return tiny_break(_default_predicate); }

  bool mini_break(const auto &predicate) {
    if (!should_mini_break())
      return false;

    const auto duration = mini_break_settings.duration<std::chrono::minutes>();
    Debug::Info << "Taking a mini break for " << duration.count() << " minutes"
                << std::endl;

    const auto end = clock::now() + duration;
    while (end > clock::now()) {
      if (predicate())
        break;

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    const auto interval = mini_break_settings.interval<std::chrono::minutes>();
    next_mini_break = clock::now() + interval;
    Debug::Info << "Next mini break in " << interval.count() << " minutes"
                << std::endl;
    return true;
  }

  bool mini_break() { return mini_break(_default_predicate); }

  bool short_break(const auto &predicate) {
    if (!should_short_break())
      return false;

    const auto duration = short_break_settings.duration<std::chrono::minutes>();
    Debug::Info << "Taking a short break for " << duration.count() << " minutes"
                << std::endl;

    const auto end = clock::now() + duration;
    while (end > clock::now()) {
      if (predicate())
        break;

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    const auto interval = short_break_settings.interval<std::chrono::minutes>();
    next_short_break = clock::now() + interval;
    Debug::Info << "Next short break in " << interval.count() << " minutes"
                << std::endl;
    return true;
  }

  bool short_break() { return short_break(_default_predicate); }

  bool long_break(const auto &predicate) {
    if (!should_long_break())
      return false;

    const auto duration = long_break_settings.duration<std::chrono::hours>();
    Debug::Info << "Taking a long break for " << duration.count() << " hours"
                << std::endl;

    const auto end = clock::now() + duration;
    while (end > clock::now()) {
      if (predicate())
        break;

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    const auto interval = long_break_settings.interval<std::chrono::hours>();
    next_long_break = clock::now() + interval;
    Debug::Info << "Next long break in " << interval.count() << " hours"
                << std::endl;
    return true;
  }

  bool long_break() { return long_break(_default_predicate); }

  bool take_break(const auto &predicate) {
    if (should_tiny_break())
      return tiny_break(predicate);

    if (should_mini_break())
      return mini_break(predicate);

    if (should_short_break())
      return short_break(predicate);

    if (should_long_break())
      return long_break(predicate);

    return false;
  }

  bool take_break() { return take_break(_default_predicate); }

private:
  static bool _default_predicate() {
    if (Internal::GetFocused())
      Antiban::MouseOffClient(true);

    Wait(1000);
    return false;
  }
};
} // namespace as