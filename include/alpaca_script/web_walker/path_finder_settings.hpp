#pragma once

#include <Game/Interfaces/GameTabs/Stats.hpp>

namespace as::web_walker {
  class path_finder_settings {
  public:
    using item = std::pair<std::string, std::int32_t>;

    bool use_shortcuts = true;
    bool use_teleport = true;

    std::int32_t agility_level = 1;
    std::int32_t magic_level = 1;
    std::int32_t ranged_level = 1;
    std::int32_t strength_level = 1;

    bool use_transportations = true;

    std::vector<item> items = {};
    
    path_finder_settings() = default;
  };

  path_finder_settings get_player_settings() {
    path_finder_settings result;
    result.agility_level = Stats::GetRealLevel(Stats::AGILITY);
    return result;
  }
}