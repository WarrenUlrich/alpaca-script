#pragma once

#include <Game/Core.hpp>

namespace as {
class essence_pouch {
public:
  enum essence_type {
    pure_essence,
    guardian_essence,
  };

  std::int32_t id;
  std::int32_t degraded_id;
  std::int32_t max_capacity;
  std::int32_t max_degraded_capacity;
  std::int32_t runecrafting_level;

  essence_pouch(std::int32_t id, std::int32_t degraded_id,
                std::int32_t max_capacity, std::int32_t max_degraded_capacity,
                std::int32_t runecrafting_level)
      : id(id), degraded_id(degraded_id), max_capacity(max_capacity),
        max_degraded_capacity(max_degraded_capacity),
        runecrafting_level(runecrafting_level), _current_count(0) {}

  essence_pouch(const essence_pouch &other)
      : id(other.id), degraded_id(other.degraded_id),
        max_capacity(other.max_capacity),
        max_degraded_capacity(other.max_degraded_capacity),
        runecrafting_level(other.runecrafting_level),
        _current_count(other._current_count) {}

  essence_pouch(essence_pouch &&other)
      : id(other.id), degraded_id(other.degraded_id),
        max_capacity(other.max_capacity),
        max_degraded_capacity(other.max_degraded_capacity),
        runecrafting_level(other.runecrafting_level),
        _current_count(other._current_count) {}

  Interactable::Item get_item() {
    return Inventory::GetItem(std::vector<std::int32_t>{id, degraded_id});
  }

  std::int32_t get_current_count() const { return _current_count; }

  std::int32_t get_max_capacity() const {
    return degraded() ? max_degraded_capacity : max_capacity;
  }

  std::int32_t get_remaining_capacity() const {
    return get_max_capacity() - get_current_count();
  }

  bool in_inventory() const {
    return Inventory::Contains(id) ||
           (degraded_id != -1 && Inventory::Contains(degraded_id));
  }

  bool degraded() const {
    return (degraded_id != -1 && Inventory::Contains(degraded_id));
  }

  bool fill(essence_type type) {
    if (full())
      return true;

    auto essence_name = [&]() {
      switch (type) {
      case essence_type::pure_essence:
        return "Pure essence";
      case essence_type::guardian_essence:
        return "Guardian essence";
      }

      throw std::runtime_error("Invalid essence type");
    }();

    const auto essence = Inventory::GetItem(essence_name);
    if (!essence)
      return false;

    const auto count = Inventory::Count(essence_name);

    const auto pouch = get_item();
    if (!pouch)
      return false;

    if (!pouch.Interact("Fill"))
      return false;

    if (!WaitFunc(2000, 50,
                  [&]() { return Inventory::Count(essence_name) < count; }))
      return false;

    _current_count +=
        count < get_remaining_capacity() ? count : get_remaining_capacity();
    return true;
  }

  bool full() const {
    if (degraded())
      return _current_count >= max_degraded_capacity;
    else
      return _current_count >= max_capacity;
  }

  bool empty() {
    const auto empty_slots = Inventory::CountEmpty();
    auto pouch = Inventory::GetItem(id);
    if (!pouch) {
      pouch = Inventory::GetItem(degraded_id);
      if (!pouch)
        return false;
    }

    Interact::DownKey(Key::KEY_SHIFT);
    if (!pouch.Interact("Empty")) {
      Interact::UpKey(Key::KEY_SHIFT);
      return false;
    }
    Interact::UpKey(Key::KEY_SHIFT);

    if (!WaitFunc(2000, 50,
                  [&]() { return Inventory::CountEmpty() < empty_slots; }))
      return false;

    if (empty_slots < _current_count) {
      _current_count -= empty_slots;
    } else {
      _current_count = 0;
    }

    return true;
  }

  bool is_empty() const { return !(_current_count > 0); }

  bool can_use() const {
    return Stats::GetCurrentLevel(Stats::RUNECRAFT) >= runecrafting_level;
  }

  static essence_pouch small;

  static essence_pouch medium;

  static essence_pouch large;

  static essence_pouch giant;

  static std::vector<essence_pouch *> all;

private:
  std::int32_t _current_count;
};

essence_pouch essence_pouch::small = essence_pouch(5509, -1, 3, 3, 1);

essence_pouch essence_pouch::medium = essence_pouch(5510, 5511, 6, 3, 25);

essence_pouch essence_pouch::large = essence_pouch(5512, 5513, 9, 3, 50);

essence_pouch essence_pouch::giant = essence_pouch(5514, 5515, 12, 3, 75);

std::vector<essence_pouch *> essence_pouch::all = {
    &essence_pouch::small, &essence_pouch::medium, &essence_pouch::large,
    &essence_pouch::giant};
} // namespace as