#pragma once

#include "obstacle.hpp"
#include <Game/Tools/Pathfinding.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <queue>
#include <unordered_map>
#include <unordered_set>

template <> struct std::hash<Tile> {
  std::size_t operator()(const Tile &tile) const {
    auto hx = std::hash<std::uint32_t>{}(tile.X);
    auto hy = std::hash<std::uint32_t>{}(tile.Y);
    auto hz = std::hash<std::uint32_t>{}(tile.Plane);

    // return hx ^ (hy << 1) ^ (hz << 2);
    return (hx ^ (hy << 1)) ^ hz;
  }
};

namespace as::web_walker {
// std::vector<std::pair<Tile, std::int32_t>> collision_data;

std::unordered_map<Tile, std::int32_t> collision_data;

void initialize(const std::filesystem::path &path_to_data) {
  if (path_to_data.empty()) {
    throw std::runtime_error("path_to_data is empty");
  }

  if (!std::filesystem::exists(path_to_data)) {
    throw std::runtime_error("path_to_data does not exist");
  }

  if (!std::filesystem::is_directory(path_to_data)) {
    throw std::runtime_error("path_to_data is not a directory");
  }

  for (const auto &dir_entry :
       std::filesystem::directory_iterator(path_to_data)) {
    if (!std::filesystem::is_regular_file(dir_entry)) {
      continue;
    }

    if (dir_entry.path().extension() != ".json") {
      continue;
    }

    std::cout << "web_walker: "
              << "Loading region " << dir_entry.path().stem() << '\n';

    auto ifs = std::ifstream(dir_entry.path());
    if (!ifs) {
      throw std::runtime_error("failed to open " + dir_entry.path().string());
    }

    auto json = nlohmann::json::parse(ifs);
    for (const auto &tile : json) {
      auto x = tile["x"].get<std::int32_t>();
      auto y = tile["y"].get<std::int32_t>();
      auto plane = tile["plane"].get<std::int32_t>();
      auto flags = tile["flags"].get<std::int32_t>();
      // collision_data.emplace(std::make_pair(Tile{x, y, plane}, flags));
      collision_data[Tile{x, y, plane}] = flags;
    }
  }
}

std::int32_t is_collision(const Tile &tile) {
  auto it = collision_data.find(tile);
  if (it != collision_data.end()) {
    return it->second;
  }

  return 0;
}

bool blocked(std::int32_t flags) {
  return (flags & Pathfinding::BLOCKED) || (flags & Pathfinding::OCCUPIED);
}

std::vector<Tile> get_neighbors(const Tile &tile) {
  std::vector<Tile> result;
  auto flags = is_collision(tile);
  if (blocked(flags)) {
    return result;
  }

  //north 
  if (!(flags & Pathfinding::NORTH)) {
    auto north = tile + Tile(0, 1, 0);
    if (!blocked(is_collision(north))) {
      result.emplace_back(north);
    }
  }

  // east
  if (!(flags & Pathfinding::EAST)) {
    auto east = tile + Tile(1, 0, 0);
    if (!blocked(is_collision(east))) {
      result.emplace_back(east);
    }
  }

  // south
  if (!(flags & Pathfinding::SOUTH)) {
    auto south = tile + Tile(0, -1, 0);
    if (!blocked(is_collision(south))) {
      result.emplace_back(south);
    }
  }

  // west
  if (!(flags & Pathfinding::WEST)) {
    auto west = tile + Tile(-1, 0, 0);
    if (!blocked(is_collision(west))) {
      result.emplace_back(west);
    }
  }
  return result;
}

class node {
public:
  Tile tile;
  std::shared_ptr<node> parent;

  node(const Tile &tile, std::shared_ptr<node> parent)
      : tile(tile), parent(parent) {}
};

std::vector<Tile> find_path(const Tile &start, const Tile &end) {
  // BFS
  std::queue<std::shared_ptr<node>> queue;
  std::unordered_set<Tile> visited;
  std::unordered_map<Tile, std::shared_ptr<node>> nodes;
  std::vector<Tile> result;

  queue.emplace(std::make_shared<node>(start, nullptr));
  visited.insert(start);
  nodes[start] = queue.front();

  while (!queue.empty()) {
    auto current = queue.front();
    queue.pop();

    if (current->tile == end) {
      while (current != nullptr) {
        result.emplace_back(current->tile);
        current = current->parent;
      }
      std::reverse(result.begin(), result.end());
      return result;
    }

    for (const auto &neighbor : get_neighbors(current->tile)) {
      if (visited.find(neighbor) != visited.end()) {
        continue;
      }

      auto n = std::make_shared<node>(neighbor, current);
      queue.emplace(n);
      visited.insert(neighbor);
      nodes[neighbor] = n;
    }
  }

  return result;
}

// class node {
// public:
//   Tile tile;

//   std::shared_ptr<node> parent;

//   node(Tile tile, std::shared_ptr<node> parent) : tile(tile), parent(parent)
//   {}
// };

// std::vector<Tile> find_path(const Tile &start, const Tile &end) {
//   std::queue<std::shared_ptr<node>> queue;

//   auto nodes = std::unordered_map<Tile, std::shared_ptr<node>>{};

//   auto start_node = std::make_shared<node>(start, nullptr);
//   nodes[start] = start_node;

//   queue.push(start_node);

//   while (!queue.empty()) {
//     auto &current_node = queue.front();

//     if (current_node->tile == end) {
//       break;
//     }

//     for (auto &neighbor : get_neighbors(current_node->tile)) {
//       if (nodes.find(neighbor) != nodes.end()) {
//         continue;
//       }

//       auto new_node = std::make_shared<node>(neighbor, current_node);

//       nodes[neighbor] = new_node;
//       queue.push(new_node);
//     }

//     queue.pop();
//   }

//   std::vector<Tile> result{};

//   auto current_node = nodes[end];
//   while (current_node != nullptr) {
//     result.emplace_back(current_node->tile);
//     current_node = current_node->parent;
//   }

//   std::reverse(result.begin(), result.end());
//   return result;
// }
} // namespace as::web_walker