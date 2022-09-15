#pragma once

#include <Game/Core.hpp>
#include <alpaca_script/web_walker/obstacle.hpp>

namespace as::web_walker::obstacles {
class gate : public obstacle {
public:
  Tile closed_position;

  gate(Tile closed_position, Tile destination)
      : obstacle(destination), closed_position(closed_position) {}

  bool handle() const override {
    const auto gate = WallObjects::Get(closed_position);
    if (!gate)
      return true; // gate open?

    if (!gate.Interact("Open"))
      return false;

    return WaitFunc(2000, 50,
                    [&]() { return !WallObjects::Get(closed_position); });
  }
};
} // namespace as::web_walker::obstacles