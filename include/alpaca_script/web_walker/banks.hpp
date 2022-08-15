#pragma once

#include "bank.hpp"

namespace as::web_walker::banks {

static bank_booth lumbridge_bank = bank_booth(Tile(3208, 3219, 2), Area());

static bank_booth varrock_east_bank = bank_booth(Tile(3253, 3421, 0), Area());

static bank_booth varrock_west_bank = bank_booth(
    Tile(3183, 3440, 0), Area(Tile(3180, 3447, 0), Tile(3185, 3433, 0)));

static bank_booth grand_exchange = bank_booth(Tile(3164, 3486, 0), Area());

static bank_booth edgeville_bank = bank_booth(
    Tile(3093, 3490, 0), Area(Tile(3091, 3499, 0), Tile(3098, 3488, 0)));

const std::vector<std::reference_wrapper<const bank>> &all() {
  static std::vector<std::reference_wrapper<const bank>> banks = {
      std::cref(lumbridge_bank), std::cref(varrock_east_bank),
      std::cref(varrock_west_bank), std::cref(grand_exchange), std::cref(edgeville_bank)};
  return banks;
}

const bank &get_closest(const Tile &tile) {
  const auto &banks = all();

  auto closest = banks.begin();
  for (auto it = banks.begin(); it != banks.end(); ++it) {
    if (!it->get().accessible())
      continue;

    if (it->get().distance_from(tile) < closest->get().distance_from(tile)) {
      closest = it;
    }
  }
  return *closest;
}
} // namespace as::web_walker::banks
