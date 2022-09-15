#pragma once

#include <Game/Core.hpp>

#include <alpaca_script/mouse_camera.hpp>

namespace as::web_walker {
class bank {
public:
  Tile location;
  Area area;

  bank() = default;

  bank(const bank &other) : location(other.location), area(other.area) {}

  bank(bank &&other)
      : location(std::move(other.location)), area(std::move(other.area)) {}

  bank(const Tile &location, const Area &area)
      : location(location), area(area) {}

  bank(Tile &&location, Area &&area)
      : location(std::move(location)), area(std::move(area)) {}

  std::int32_t distance_from(const Tile &tile) const {
    return location.DistanceFrom(tile);
  }

  std::int32_t distance_from(const bank &other) const {
    return distance_from(other.location);
  }

  bool in_area(const Tile &tile) const { return area.Contains(tile); }

  virtual bool accessible() const { return true; }

  virtual bool open() const = 0;
};

class bank_booth : public bank {
public:
  bank_booth() = default;

  bank_booth(const bank_booth &other) : bank(other) {}

  bank_booth(bank_booth &&other) : bank(std::move(other)) {}

  bank_booth(const Tile &location, const Area &area) : bank(location, area) {}

  bank_booth(Tile &&location, Area &&area)
      : bank(std::move(location), std::move(area)) {}

  bool open() const override {
    if (Bank::IsOpen())
      return true;

    auto obj = GameObjects::Get("Bank booth");
    if (!obj)
      return false;

    if (obj.GetVisibility() < .5)
      if (!as::mouse_camera::rotate_to(obj.GetTile()))
        return false;

    if (!obj.Interact("Bank"))
      return false;

    return WaitFunc(2500, 50, Bank::IsOpen);
  }
};
} // namespace as::web_walker