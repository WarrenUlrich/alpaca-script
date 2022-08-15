#pragma once

#include <vector>
#include <Core/Types/Tile.hpp>

#include "path_finder_settings.hpp"

namespace as::web_walker {
class teleport {
public:
  teleport() = default;
  
  virtual std::vector<Tile> get_destinations(const path_finder_settings& settings) const = 0;

  virtual bool handle(const Tile& dest) const = 0;

  virtual ~teleport() = default;
};
} // namespace as::web_walker