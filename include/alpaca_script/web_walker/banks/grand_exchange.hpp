#pragma once

#include <Game/Core.hpp>
#include <alpaca_script/mouse_camera.hpp>
#include <alpaca_script/web_walker/bank.hpp>

namespace as::web_walker::banks {
class grand_exchange_bank : public bank {
public:
  grand_exchange_bank()
      : bank(Tile(3164, 3486, 0),
             Area(Tile(3157, 3496, 0), Tile(3171, 3483, 0))) {}

  bool open() const override {
    if (Bank::IsOpen())
      return true;

    const auto booths = WallObjects::GetAll([](const auto &booth) {
      if (booth.GetName() != "Grand Exchange booth")
        return false;

      const auto actions = booth.GetInfo().GetActions();

      return std::find(actions.begin(), actions.end(), "Bank") != actions.end();
    });

    if (booths.empty())
      return false;

    const auto &obj = booths.front();
    if (obj.GetVisibility() < .5)
      if (!as::mouse_camera::rotate_to(obj.GetTile()))
        return false;

    if (!obj.Interact("Exchange"))
      return false;

    return WaitFunc(2500, 50, Bank::IsOpen);
  }
};

} // namespace as::web_walker::banks