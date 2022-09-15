#pragma once

#include <Game/Core.hpp>
#include <alpaca_script/web_walker/obstacle.hpp>

namespace as::web_walker::obstacles {
class wilderness_gate : public obstacle {
public:
  Tile closed_position;

  wilderness_gate(Tile closed_position, Tile destination)
      : obstacle(destination), closed_position(closed_position) {}

  bool handle() const override {
    const auto gate = WallObjects::Get(closed_position);
    if (!gate)
      return true; // gate open?

    if (!gate.Interact("Open"))
      return false;

    WaitFunc(2000, 50, [&]() {
      if (Minimap::GetPosition() == destination)
        return true;

      const auto widget = Widgets::Get(475, 11);
      if (!widget)
        return false;

      if (!widget.Interact())
        return false;

      return WaitFunc(2000, 50,
                      [&]() { return Minimap::GetPosition() == destination; });
    });

    return WaitFunc(3000, 50,
                    [&]() { return Minimap::GetPosition() == destination; });
  }
};
} // namespace as::web_walker::obstacles