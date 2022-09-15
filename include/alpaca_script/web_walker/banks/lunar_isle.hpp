#pragma once

#include <Game/Core.hpp>
#include <alpaca_script/web_walker/bank.hpp>

namespace as::web_walker::banks {
class lunar_isle_bank : public bank {
public:
  lunar_isle_bank()
      : bank(Tile(2099, 3918, 0),
             Area(Tile(2097, 3921, 0), Tile(2104, 3917, 0))) {}

  bool accessible() const override { return true; }

  bool open() const override {
    if (Bank::IsOpen())
      return true;

    const auto booth = GameObjects::Get(Tile(2099, 3920, 0));
    if (!booth)
      return false;

    if (booth.GetVisibility() < .6)
      as::mouse_camera::rotate_to(booth.GetTile());

    if (!booth.Interact("Bank"))
      return false;

    return WaitFunc(2500, 50, Bank::IsOpen);
  }
};
} // namespace as::web_walker::banks