#pragma once

#include <Game/Core.hpp>
#include <alpaca_script/web_walker/obstacle.hpp>

namespace as::web_walker::obstacles {
class ladder : public obstacle {
public:
  Tile object_tile;

  ladder(Tile object_tile, Tile destination)
      : obstacle(destination), object_tile(object_tile) {}

  bool handle() const override {
    auto obj = GameObjects::Get(object_tile);

    if (!obj)
      return false;

    if (!obj.Interact("Climb-up"))
      return false;

    return WaitFunc(2500, 50,
                    [&]() { return Minimap::GetPosition() == destination; });
  }
};
}; // namespace as::web_walker::obstacles