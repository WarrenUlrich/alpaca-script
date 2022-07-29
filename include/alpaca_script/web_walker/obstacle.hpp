#pragma once
#include <Game/Core.hpp>

namespace as::web_walker {
class obstacle {
public:
  virtual bool can_pass() const = 0;

  virtual bool handle() = 0;
};

class door_obstacle : public obstacle {
public:
  Tile closed_position;

  door_obstacle(const Tile &closed_position)
      : closed_position(closed_position) {}

  door_obstacle(const door_obstacle &other)
      : closed_position(other.closed_position) {}

  door_obstacle(door_obstacle &&other)
      : closed_position(std::move(other.closed_position)) {}

  door_obstacle &operator=(const door_obstacle &other) {
    closed_position = other.closed_position;
    return *this;
  }

  door_obstacle &operator=(door_obstacle &&other) {
    closed_position = std::move(other.closed_position);
    return *this;
  }

  bool can_pass() const override { return true; }

  bool handle() override {
    auto door = WallObjects::Get(closed_position);
    if (!door) {
      return true;
    }
  }
};
} // namespace as::web_walker