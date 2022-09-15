#pragma once

#include <alpaca_script/web_walker/bank.hpp>

namespace as::web_walker::banks {
class fossil_island_bank : public bank {
public:
  fossil_island_bank()
      : bank(Tile(3740, 3804, 0),
             Area(Tile(3743, 3805, 0), Tile(3739, 3802, 0))) {}

  bool accessible() const override { return true; }

  bool open() const override {
    if (Bank::IsOpen())
      return true;

    const auto chest = GameObjects::Get("Bank chest");
    if (!chest)
      return false;
    
    if (chest.GetVisibility() < .6)
      as::mouse_camera::rotate_to(chest.GetTile());
    
    if (!chest.Interact("Use"))
      return false;
    
    return WaitFunc(2500, 50, Bank::IsOpen);
  }
};
} // namespace as::web_walker::banks