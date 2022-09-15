#pragma once

#include <Game/Core.hpp>
#include <alpaca_script/web_walker/obstacle.hpp>
#include <alpaca_script/mouse_camera.hpp>

namespace as::web_walker::obstacles {
class trapdoor : public obstacle {
public:
  Tile object_tile;

  trapdoor(Tile object_tile, Tile destination)
      : obstacle(destination), object_tile(object_tile) {}

  bool handle() const override {
    auto obj = GroundObjects::Get(object_tile);
    if (!obj)
      return false;

    constexpr auto open_id = 1581;
    constexpr auto closed_id = 1579;

    if (obj.GetID() == closed_id) {
      if (obj.GetVisibility() < .6)
        as::mouse_camera::rotate_to(object_tile);

      if (!obj.Interact("Open"))
        return false;

      if (!WaitFunc(2000, 50, [&]() {
            obj = GroundObjects::Get(object_tile);
            return obj && obj.GetID() == open_id;
          }))
        return false;
    }

    if (obj.GetVisibility() < .6)
      as::mouse_camera::rotate_to(object_tile);

    if (!obj.Interact("Climb-down"))
      return false;

    return WaitFunc(2500, 50,
                    [&]() { return Minimap::GetPosition() == destination; });
  }
};
} // namespace as::web_walker::obstacles