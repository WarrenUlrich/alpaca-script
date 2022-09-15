#pragma once

#include <alpaca_script/web_walker/obstacle.hpp>

namespace as::web_walker::obstacles {
class magic_mushroomtree : public obstacle {
public:
  static std::unordered_map<Tile, std::pair<std::int32_t, std::int32_t>>
      transport_widget_map;

  magic_mushroomtree(Tile destination) : obstacle(destination) {}

  bool handle() const override {
    const auto tree = GameObjects::Get("Magic Mushtree");

    if (!tree)
      return false;

    auto transport_widget = Widgets::Get(608, 0);

    if (!transport_widget.IsVisible()) {
      if (!tree.Interact("Use"))
        return false;

      if (!WaitFunc(2250, 50, [&]() {
            transport_widget = Widgets::Get(608, 0);
            return transport_widget.IsVisible();
          }))
        return false;
    }

    const auto &[x, y] = transport_widget_map.at(destination);

    const auto widg = Widgets::Get(x, y);
    if (!widg.Interact("Continue"))
      return false;

    return WaitFunc(4000, 50,
                    [&]() { return Minimap::GetPosition() == destination; });
  }
};

std::unordered_map<Tile, std::pair<std::int32_t, std::int32_t>>
    magic_mushroomtree::transport_widget_map = {
        {Tile(3760, 3758, 0), {608, 8}},  // verdant valley
        {Tile(3676, 3871, 0), {608, 16}}, // mushroom meadow
};
} // namespace as::web_walker::obstacles