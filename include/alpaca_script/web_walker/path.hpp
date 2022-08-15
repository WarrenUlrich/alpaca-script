#pragma once

#include <variant>
#include <Game/Tools/Pathfinding.hpp>
#include "obstacle.hpp"
#include "teleport.hpp"

namespace as::web_walker {
class path {
public:
  using obstacle_step = std::pair<Tile, const obstacle *>;

  using teleport_step = std::pair<Tile, const teleport *>;

  using value_type = std::variant<Tile, obstacle_step, teleport_step>;

  using reference = value_type &;

  using vector = std::vector<value_type>;

  using iterator = vector::iterator;

  using const_iterator = vector::const_iterator;

  vector steps;

  path() = default;

  path(const path &other) : steps(other.steps) {}

  path(path &&other) : steps(std::move(other.steps)) {}

  path(const vector &steps) : steps(steps) {}

  path(vector &&steps) : steps(std::move(steps)) {}

  path &operator=(const path &other) {
    steps = other.steps;
    return *this;
  }

  path &operator=(path &&other) {
    steps = std::move(other.steps);
    return *this;
  }

  iterator insert(const iterator &pos, const value_type &value) {
    return steps.insert(pos, value);
  }

  iterator insert(const iterator &pos, value_type &&value) {
    return steps.insert(pos, std::move(value));
  }

  template <typename... Args> reference emplace_back(Args &&...args) {
    return steps.emplace_back(std::forward<Args &&>(args)...);
  }

  iterator begin() { return steps.begin(); }

  iterator end() { return steps.end(); }

  const_iterator begin() const { return steps.begin(); }

  const_iterator end() const { return steps.end(); }

  reference front() { return steps.front(); }

  reference back() { return steps.back(); }

  auto &front() const { return steps.front(); }

  auto &back() const { return steps.back(); }

  static iterator
  closest(iterator it, iterator end, const Tile &tile,
          std::int32_t distance = std::numeric_limits<std::int32_t>::max()) {
    iterator closest = end;
    for (; it != end; ++it) {
      if (auto current_tile = std::get_if<Tile>(&*it)) {
        if (current_tile->Plane != tile.Plane)
          continue;

        auto dist = current_tile->DistanceFrom(tile);

        if (dist <= distance) {
          closest = it;
          distance = dist;
        }
      }
    }

    return closest;
  }

  iterator
  closest(const Tile &tile,
          std::int32_t distance = std::numeric_limits<std::int32_t>::max()) {
    return closest(begin(), end(), tile, distance);
  }

  static const_iterator
  closest(const_iterator it, const_iterator end, const Tile &tile,
          std::int32_t distance = std::numeric_limits<std::int32_t>::max()) {
    const_iterator closest = end;
    for (; it != end; ++it) {
      if (auto current_tile = std::get_if<Tile>(&*it)) {
        if (current_tile->Plane != tile.Plane)
          continue;

        auto dist = current_tile->DistanceFrom(tile);

        if (dist < distance) {
          closest = it;
          distance = dist;
        }
      }
    }

    return closest;
  }

  const_iterator closest(
      const Tile &tile,
      std::int32_t distance = std::numeric_limits<std::int32_t>::max()) const {
    return closest(this->begin(), this->end(), tile, distance);
  }

  static iterator
  furthest(iterator it, iterator end, const Tile &tile,
           std::int32_t distance = std::numeric_limits<std::int32_t>::min()) {
    iterator furthest = end;
    // return furthest tile within the given distance
    for (; it != end; ++it) {
      if (auto current_tile = std::get_if<Tile>(&*it)) {
        if (current_tile->Plane != tile.Plane)
          continue;

        auto dist = current_tile->DistanceFrom(tile);
        if (dist >= distance)
          continue;

        furthest = it;
      }
    }

    return furthest;
  }

  iterator
  furthest(const Tile &tile,
           std::int32_t distance = std::numeric_limits<std::int32_t>::min()) {
    return furthest(begin(), end(), tile, distance);
  }

  static const_iterator
  furthest(const_iterator it, const_iterator end, const Tile &tile,
           std::int32_t distance = std::numeric_limits<std::int32_t>::min()) {
    const_iterator furthest = end;
    // return furthest tile within the given distance
    for (; it != end; ++it) {
      if (auto current_tile = std::get_if<Tile>(&*it)) {
        if (current_tile->Plane != tile.Plane)
          continue;

        auto dist = current_tile->DistanceFrom(tile);
        if (dist >= distance)
          continue;

        furthest = it;
      }
    }

    return furthest;
  }

  const_iterator furthest(
      const Tile &tile,
      std::int32_t distance = std::numeric_limits<std::int32_t>::min()) const {
    return furthest(this->begin(), this->end(), tile, distance);
  }

  static iterator furthest_reachable(iterator it, iterator end,
                                     const Tile &tile,
                                     std::int32_t distance = 0) {
    iterator furthest = end;

    auto is_reachable = [](const Tile& t) {
      return !Pathfinding::FindPathTo(t).empty();
    };

    for (; it != end; ++it) {
      if (auto current_tile = std::get_if<Tile>(&*it)) {
        if (current_tile->Plane != tile.Plane)
          continue;

        auto dist = current_tile->DistanceFrom(tile);
        if (dist >= distance)
          continue;
        
        if (!is_reachable(*current_tile))
          continue;

        furthest = it;
      }
    }

    return furthest;
  }

  std::pair<iterator, obstacle_step *> next_obstacle(iterator it,
                                                     iterator end) {
    for (; it != end; ++it) {
      if (obstacle_step *step = std::get_if<obstacle_step>(&*it)) {
        return {it, step};
      }
    }

    return {end, nullptr};
  }

  std::pair<iterator, obstacle_step *> next_obstacle() {
    return next_obstacle(begin(), end());
  }

  std::pair<const_iterator, const obstacle_step *>
  next_obstacle(const_iterator it, const_iterator end) const {
    for (; it != end; ++it) {
      if (const obstacle_step *step = std::get_if<obstacle_step>(&*it)) {
        return {it, step};
      }
    }

    return {end, nullptr};
  }

  std::pair<const_iterator, const obstacle_step *> next_obstacle() const {
    return next_obstacle(begin(), end());
  }
};
} // namespace as::web_walker