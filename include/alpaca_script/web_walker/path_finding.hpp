#pragma once

#include <queue>
#include <stdexcept>
#include <unordered_set>

#include "collision.hpp"
#include "obstacle.hpp"
#include "obstacles.hpp"
#include "path.hpp"
#include "path_finder_settings.hpp"

namespace as::web_walker {
class path_node {
public:
  path::value_type value;
  std::shared_ptr<path_node> parent;

  path_node(const path::value_type &value, std::shared_ptr<path_node> parent)
      : value(value), parent(parent) {}

  path_node(path::value_type &&value, std::shared_ptr<path_node> parent)
      : value(std::move(value)), parent(parent) {}
};

std::vector<path::value_type> get_neighbors(const Tile &tile) {
  std::vector<path::value_type> result;

  auto flags = collision_map.find(tile);
  if (flags == collision_map.end()) {
  }

  if (!(flags->second & Pathfinding::NORTH)) {
    auto north = tile + Tile(0, 1, 0);

    auto obstacles = obstacle_map.equal_range(north);

    for (auto it = obstacles.first; it != obstacles.second; ++it) {
      result.emplace_back(path::obstacle_step(north, it->second.get()));
    }
  }

  return result;
}

bool blocked(std::int32_t flags) {
  return (flags & Pathfinding::BLOCKED) || (flags & Pathfinding::OCCUPIED);
}

std::int32_t is_collision(const Tile &tile) {
  auto flags = collision_map.find(tile);
  if (flags != collision_map.end()) {
    return flags->second;
  }

  return 0;
}

void visit_neighbors(const Tile &tile, auto &&func) {
  auto flags = is_collision(tile);

  if (!(flags & Pathfinding::NORTH)) {
    auto north = tile + Tile(0, 1, 0);

    auto obstacles = obstacle_map.equal_range(north);
    if (obstacles.first != obstacle_map.end()) {
      for (auto it = obstacles.first; it != obstacles.second; ++it) {
        func(path::obstacle_step(north, it->second.get()));
      }
    } else {
      if (!blocked(is_collision(north))) {
        func(north);
      }
    }
  }

  // east
  if (!(flags & Pathfinding::EAST)) {
    auto east = tile + Tile(1, 0, 0);

    auto obstacles = obstacle_map.equal_range(east);
    if (obstacles.first != obstacle_map.end()) {
      for (auto it = obstacles.first; it != obstacles.second; ++it) {
        func(path::obstacle_step(east, it->second.get()));
      }
    } else {
      if (!blocked(is_collision(east))) {
        func(east);
      }
    }
  }

  // south
  if (!(flags & Pathfinding::SOUTH)) {
    auto south = tile + Tile(0, -1, 0);

    auto obstacles = obstacle_map.equal_range(south);
    if (obstacles.first != obstacle_map.end()) {
      for (auto it = obstacles.first; it != obstacles.second; ++it) {
        func(path::obstacle_step(south, it->second.get()));
      }
    } else {
      if (!blocked(is_collision(south))) {
        func(south);
      }
    }
  }

  // west
  if (!(flags & Pathfinding::WEST)) {
    auto west = tile + Tile(-1, 0, 0);

    auto obstacles = obstacle_map.equal_range(west);
    if (obstacles.first != obstacle_map.end()) {
      for (auto it = obstacles.first; it != obstacles.second; ++it) {
        func(path::obstacle_step(west, it->second.get()));
      }
    } else {
      if (!blocked(is_collision(west))) {
        func(west);
      }
    }
  }
}

path find_path(const Tile &start, const Tile &dest,
               const path_finder_settings &settings = path_finder_settings()) {

  std::queue<std::shared_ptr<path_node>> queue;
  std::unordered_set<Tile> visited;

  queue.emplace(std::make_shared<path_node>(start, nullptr));

  while (!queue.empty()) {
    auto current = std::move(queue.front());
    queue.pop();

    if (auto tile = std::get_if<Tile>(&current->value)) {
      if (*tile == dest) {
        path result;

        while (current != nullptr) {
          result.emplace_back(current->value);
          current = current->parent;
        }

        std::reverse(result.begin(), result.end());
        return result;
      }
    }

    auto tile = std::visit(
        [&](auto &&value) {
          using value_t = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<value_t, Tile>) {
            return value;
          } else if constexpr (std::is_same_v<value_t, path::obstacle_step>) {
            return value.second->destination;
          }

          throw std::runtime_error("unexpected value type");
          return Tile();
        },
        current->value);

    visit_neighbors(tile, [&](auto &&neighbor) {
      using neighbor_t = std::decay_t<decltype(neighbor)>;

      if constexpr (std::is_same_v<neighbor_t, Tile>) {
        if (visited.find(neighbor) != visited.end())
          return;

        queue.emplace(std::make_shared<path_node>(neighbor, current));
        visited.insert(neighbor);
      } else if constexpr (std::is_same_v<neighbor_t, path::obstacle_step>) {
        if (visited.find(neighbor.second->destination) != visited.end())
          return;

        if (!neighbor.second->can_handle(settings))
          return;

        queue.emplace(std::make_shared<path_node>(neighbor, current));
        visited.insert(neighbor.second->destination);
      }
    });
  }

  throw std::runtime_error("no path found");
}

bool walk_path(const path &path, std::int32_t distance = 4) {
  if (!Mainscreen::IsLoggedIn())
    return false;

  auto player = Players::GetLocal();
  if (!player)
    return false;

  auto player_tile = player.GetTile();
  auto closest = path.closest(player_tile, 10);
  if (closest == path.end())
    return false;

  std::int32_t attempts = 0;
  auto obstacle_iter = path.next_obstacle(closest, path.end());
  while (attempts <= 5) {
    if (!Mainscreen::IsLoggedIn())
      return false;

    player = Players::GetLocal();
    if (!player)
      return false;

    player_tile = player.GetTile();
    auto end_tile = std::get<Tile>(path.back());

    if (player_tile.DistanceFrom(end_tile) <= 2)
      return true;

    if (obstacle_iter.first != path.end()) {
      auto obs = obstacle_iter.second;
      if (obs->first.DistanceFrom(player_tile) <= 2) {
        if (!obs->second->handle()) {
          attempts++;
          if (attempts > 5)
            return false;

          std::cout << "Failed to handle obstacle, trying again.\n";
          closest = path.closest(player_tile, 10);
          if (closest == path.end())
            return false;

          obstacle_iter = path.next_obstacle(closest, path.end());
          continue;
        }

        obstacle_iter =
            path.next_obstacle(std::next(obstacle_iter.first), path.end());
        continue;
      }
    }

    auto furthest =
        path.furthest(closest, obstacle_iter.first, player.GetTile(), 10);

    if (furthest == path.end())
      return false;

    const auto reachable = [](const Tile &t) {
      // Check local pathfinding.
      return !Pathfinding::FindNodePathTo(t).empty();
    };

    if (auto tile = std::get_if<Tile>(&*furthest)) {
      if (!reachable(*tile)) {
        std::cout << "Not reachable\n";
        attempts++;
        if (attempts > 5)
          return false;

        closest = path.closest(player_tile, 10);
        if (closest == path.end())
          return false;

        obstacle_iter = path.next_obstacle(closest, path.end());
        continue;
      }

      if (!Mainscreen::IsTileOn(*tile)) {
        if (!Camera::RotateTo(*tile, Camera::NORTH, UniformRandom(10, 30)))
          continue;
      }

      if (!Mainscreen::ClickTile(*tile))
        continue;

      if (!WaitFunc(1000, 50, Mainscreen::IsMoving))
        continue;

      const auto dest = Minimap::GetDestination();
      if (!dest)
        continue;

      if (WaitFunc(10000, 50, [&]() {
            return Players::GetLocal().GetTile().DistanceFrom(dest) < distance;
          })) {
        return true;
      }
    } else {
      std::cout << "Furthest is not a tile\n";
    }
  }
  return true;
}
} // namespace as::web_walker