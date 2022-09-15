#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "obstacle.hpp"
#include "obstacles/gate.hpp"
#include "obstacles/ladder.hpp"
#include "obstacles/magic_mushtree.hpp"
#include "obstacles/trapdoor.hpp"
#include "obstacles/wilderness_gate.hpp"

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

  // == Edgeville ==

  // Trapdoor Edgeville -> Edgeville dungeon
  result.emplace(Tile(3097, 3468, 0), std::make_unique<obstacles::trapdoor>(
                                          Tile(3097, 3468, 0), // Object Tile
                                          Tile(3096, 9867, 0)  // Destination
                                          ));

  // == Edgeville dungeon ==

  // Ladder Edgeville Dungeon -> Edgeville
  result.emplace(Tile(3096, 9867, 0), std::make_unique<obstacles::ladder>(
                                          Tile(3096, 9867, 0), // Object
                                          Tile(3096, 3468, 0)));

  // First gate
  result.emplace(Tile(3103, 9909, 0), std::make_unique<obstacles::gate>(
                                          Tile(3103, 9909, 0), // Object
                                          Tile(3104, 9909, 0)  // Destination
                                          ));
  // Wilderness entrance gate
  result.emplace(Tile(3131, 9917, 0),
                 std::make_unique<obstacles::wilderness_gate>(
                     Tile(3131, 9917, 0), // Object
                     Tile(3131, 9918, 0)  // Destination
                     ));
  // Chaos druids gate
  result.emplace(Tile(3106, 9944, 0), std::make_unique<obstacles::gate>(
                                          Tile(3106, 9944, 0), // Object
                                          Tile(3106, 9945, 0)  // Destination
                                          ));

  // Air obelisk ladder (climb up)
  result.emplace(Tile(3088, 9970, 0), std::make_unique<obstacles::ladder>(
                                          Tile(3088, 9971, 0), // Object Tile
                                          Tile(3088, 3570, 0)  // Destination
                                          ));

  // == Wilderness ==

  // Lava dragons gate

  // Entering
  result.emplace(Tile(3201, 3856, 0), std::make_unique<obstacles::gate>(
                                          Tile(3201, 3856, 0), // Object
                                          Tile(3201, 3855, 0)  // Destination
                                          ));

  // Exiting
  result.emplace(Tile(3201, 3855, 0), std::make_unique<obstacles::gate>(
                                          Tile(3201, 3856, 0), // Object
                                          Tile(3201, 3856, 0)  // Destination
                                          ));
  // == Fossil Island ==
  // Mushroom Meadow mushtree
  result.emplace(
      Tile(3676, 3871, 0),
      std::make_unique<obstacles::magic_mushroomtree>(Tile(3760, 3758, 0)));

  // Verdant valley mushtree
  result.emplace(
      Tile(3757, 3757, 0),
      std::make_unique<obstacles::magic_mushroomtree>(Tile(3676, 3871, 0)));

  return result;
}();
} // namespace as::web_walker