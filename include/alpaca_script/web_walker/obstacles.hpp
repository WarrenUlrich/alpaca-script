#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "obstacle.hpp"

namespace as::web_walker {

using obstacle_map_t = std::unordered_multimap<Tile, std::unique_ptr<obstacle>>;

obstacle_map_t obstacle_map = []() {
  obstacle_map_t result;

  // == Lumbridge ==

  // -- General Store --

  // Door (entering)
  result.emplace(
      Tile(3215, 3245, 0),
      std::make_unique<door_obstacle>(Tile(3215, 3245, 0), // Closed position
                                      Tile(3214, 3245, 0)  // Destination
                                      ));

  // Door (exiting)
  result.emplace(
      Tile(3214, 3245, 0),
      std::make_unique<door_obstacle>(Tile(3215, 3245, 0), // Closed position
                                      Tile(3215, 3245, 0)  // Destination
                                      ));
  // -- Lumbridge Castle --

  // kitchen trapdoor (climb down)
  result.emplace(Tile(3209, 3216, 0),
                 std::make_unique<ground_object_obstacle>(
                     Tile(3209, 3216, 0), // Object tile
                     "Climb down",        // Action
                     Tile(3210, 9616, 0)  // Destination tile
                     ));

  // basement ladder (climb up)
  result.emplace(Tile(3209, 9616, 0),
                 std::make_unique<game_object_obstacle>(
                     Tile(3209, 9616, 0), // Object tile
                     "Climb-up",          // Action
                     Tile(3210, 3216, 0)  // Destination tile
                     ));

  // first floor south stairs (climb up)
  result.emplace(Tile(3205, 3208, 0), std::make_unique<game_object_obstacle>(
                                          Tile(3205, 3208, 0), // Object Tile
                                          "Climb-up",          // Action
                                          Tile(3205, 3209, 1)  // Destination
                                          ));

  // second floor south stairs (climb up)
  result.emplace(Tile(3205, 3208, 1), std::make_unique<game_object_obstacle>(
                                          Tile(3205, 3208, 1), // Object Tile
                                          "Climb-up",          // Action
                                          Tile(3205, 3209, 2)  // Destination
                                          ));

  // second floor south stairs (climb down)
  result.emplace(Tile(3205, 3209, 1), std::make_unique<game_object_obstacle>(
                                          Tile(3205, 3208, 1), // Object Tile
                                          "Climb-down",        // Action
                                          Tile(3206, 3208, 0)  // Destination
                                          ));

  // third floor south stairs (climb down)
  result.emplace(Tile(3205, 3208, 2), std::make_unique<game_object_obstacle>(
                                          Tile(3205, 3208, 2), // Object Tile
                                          "Climb-down",        // Action
                                          Tile(3206, 3208, 1)  // Destination
                                          ));

  // == Varrock ==

  // == Grand Exchange ==
  // Underwall tunnel GE -> Edgeville
  result.emplace(Tile(3141, 3513, 0),
                 std::make_unique<game_object_shortcut<21>>(
                     Tile(3141, 3513, 0), // Object Tile
                     "Climb-into",        // Action
                     Tile(3138, 3516, 0)  // Destination
                     ));

  // Underwall tunnel Edgeville -> GE
  result.emplace(Tile(3138, 3516, 0),
                 std::make_unique<game_object_shortcut<21>>(
                     Tile(3138, 3516, 0), // Object Tile
                     "Climb-into",        // Action
                     Tile(3142, 3513, 0)  // Destination
                     ));
  return result;
}();
} // namespace as::web_walker