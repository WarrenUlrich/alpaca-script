#pragma once

#include <Core/Types/Tile.hpp>
#include <vector>

#include "path_finder_settings.hpp"

namespace as::web_walker {
class obstacle {
public:
  Tile destination;

  obstacle() = default;

  obstacle(const obstacle &other) : destination(other.destination) {}

  obstacle(obstacle &&other) : destination(std::move(other.destination)) {}

  obstacle(Tile destination) : destination(destination) {}

  Tile get_destination() const { return destination; }

  virtual bool can_handle(const path_finder_settings &settings) const { return true; }

  virtual bool handle() const = 0;

  virtual ~obstacle() = default;
};

class door_obstacle : public obstacle {
public:
  Tile closed_position;

  door_obstacle() = default;

  door_obstacle(const door_obstacle &other)
      : obstacle(other), closed_position(other.closed_position) {}

  door_obstacle(door_obstacle &&other)
      : obstacle(std::move(other)),
        closed_position(std::move(other.closed_position)) {}

  door_obstacle(Tile closed_position, Tile destination)
      : obstacle(destination), closed_position(closed_position) {}

  bool handle() const override {
    auto obj = WallObjects::Get(closed_position);
    if (!obj)
      return true; // door open?

    obj.Interact("Open");
    WaitFunc(2000, 50, [&]() { return !WallObjects::Get(closed_position); });

    return true;
  }
};
class game_object_obstacle : public obstacle {
public:
  Tile tile;
  std::string action;

  game_object_obstacle() = default;

  game_object_obstacle(const game_object_obstacle &other)
      : obstacle(other), tile(other.tile), action(other.action) {}

  game_object_obstacle(game_object_obstacle &&other)
      : obstacle(std::move(other)), tile(std::move(other.tile)),
        action(std::move(other.action)) {}

  game_object_obstacle(const Tile &tile, const std::string &action,
                       const Tile &destination)
      : obstacle(destination), tile(tile), action(action) {}

  game_object_obstacle(Tile &&tile, std::string &&action, Tile &&destination)
      : obstacle(std::move(destination)), tile(std::move(tile)),
        action(std::move(action)) {}

  virtual bool handle() const override {
    auto player = Players::GetLocal();
    if (!player) {
      return false;
    }

    auto obj = GameObjects::Get(tile);
    if (!obj) {
      return false;
    }

    while (obj.GetVisibility() < 0.5) {
      Camera::RotateTo(obj.GetTile(), 0);
    }

    if (!obj.Interact(action))
      return false;

    if (!WaitFunc(2000, 50, [&]() {
          return (player.GetTile().DistanceFrom(destination) < 2 &&
                  player.GetTile().Plane == destination.Plane);
        })) {
      return false;
    }

    return true;
  }
};

class ground_object_obstacle : public obstacle {
public:
  Tile tile;
  std::string action;

  ground_object_obstacle() = default;

  ground_object_obstacle(const ground_object_obstacle &other)
      : obstacle(other), tile(other.tile), action(other.action) {}

  ground_object_obstacle(ground_object_obstacle &&other)
      : obstacle(std::move(other)), tile(std::move(other.tile)),
        action(std::move(other.action)) {}

  ground_object_obstacle(const Tile &tile, const std::string &action,
                         const Tile &destination)
      : obstacle(destination), tile(tile), action(action) {}

  ground_object_obstacle(Tile &&tile, std::string &&action, Tile &&destination)
      : obstacle(std::move(destination)), tile(std::move(tile)),
        action(std::move(action)) {}

  virtual bool handle() const override {
    auto obj = GroundObjects::Get(tile);
    if (!obj) {
      return false;
    }
    return true;
  }
};

template <std::int32_t agility_level> class shortcut : public obstacle {
public:
  shortcut() = default;

  shortcut(const shortcut &other) : obstacle(other.destination) {}

  shortcut(shortcut &&other) : obstacle(std::move(other.destination)) {}

  shortcut(Tile destination) : obstacle(destination) {}

  bool can_handle(const path_finder_settings &settings) const override {
    return settings.use_shortcuts && settings.agility_level >= agility_level;
  }
};

template <std::int32_t agility_level>
class game_object_shortcut : public shortcut<agility_level> {
public:
  Tile tile;
  std::string action;

  game_object_shortcut() = default;

  game_object_shortcut(const game_object_shortcut &other)
      : shortcut<agility_level>(other), tile(other.tile), action(other.action) {
  }

  game_object_shortcut(game_object_shortcut &&other)
      : shortcut<agility_level>(std::move(other)), tile(std::move(other.tile)),
        action(std::move(other.action)) {}

  game_object_shortcut(const Tile &tile, const std::string &action,
                       const Tile &destination)
      : shortcut<agility_level>(destination), tile(tile), action(action) {}

  bool handle() const override {
    auto player = Players::GetLocal();
    if (!player) {
      return false;
    }

    for (auto player_tile = player.GetTile();
         player_tile.DistanceFrom(this->destination) > 2 &&
         player_tile.Plane == this->destination.Plane;
         player_tile = player.GetTile()) {
      if (Mainscreen::IsMoving() || Mainscreen::IsAnimating()) {
        Wait(50);
        continue;
      }

      auto obj = GameObjects::Get(tile);
      if (!obj) {
        return false;
      }

      while (obj.GetVisibility() < 0.5) {
        Camera::RotateTo(obj.GetTile(), 0);
      }

      obj.Interact(action);
      WaitFunc(750, 50, []() {
        return Mainscreen::IsMoving() || Mainscreen::IsAnimating();
      });
    }

    return true;
  }
};

template <std::int32_t agility_level>
class ground_object_shortcut : public shortcut<agility_level> {
public:
  Tile tile;
  std::string action;

  ground_object_shortcut() = default;

  ground_object_shortcut(const ground_object_shortcut &other)
      : shortcut<agility_level>(other), tile(other.tile), action(other.action) {
  }

  ground_object_shortcut(ground_object_shortcut &&other)
      : shortcut<agility_level>(std::move(other)), tile(std::move(other.tile)),
        action(std::move(other.action)) {}

  ground_object_shortcut(const Tile &tile, const std::string &action,
                         const Tile &destination)
      : shortcut<agility_level>(destination), tile(tile), action(action) {}

  bool handle() const override {
    auto player = Players::GetLocal();
    if (!player) {
      return false;
    }

    auto player_tile = player.GetTile();
    while (player_tile.DistanceFrom(tile) > 3) {
      auto obj = GroundObjects::Get(player_tile);
      if (!obj) {
        return false;
      }

      obj.Interact(action);
    }

    return true;
  }
};

} // namespace as::web_walker