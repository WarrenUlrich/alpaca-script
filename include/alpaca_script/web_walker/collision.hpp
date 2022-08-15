#pragma once

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <Core/Types/Tile.hpp>

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
  std::unordered_map<Tile, std::int32_t> collision_map;

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
      collision_map[Tile{x, y, plane}] = flags;
    }
  }
}

void initialize() {
  const char *user_profile = std::getenv("USERPROFILE");
  if (user_profile == nullptr) {
    throw std::runtime_error("failed to get USERPROFILE");
  }

  auto path_to_data =
      std::filesystem::path(user_profile) / "AlpacaBot" / "Collision Data";
  initialize(path_to_data);
}
}