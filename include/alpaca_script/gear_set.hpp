#pragma once

#include <Game/Core.hpp>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace as {

template <typename Identifier>
requires std::is_same_v<Identifier, std::string> ||
    std::is_same_v<Identifier, std::string_view> ||
    std::is_same_v<Identifier, std::int32_t>
class equippable {
public:
  std::string action;
  std::vector<Identifier> ids;

  equippable() = default;

  equippable(const std::string &action, const std::vector<Identifier> &ids)
      : action(action), ids(ids) {}

  equippable(std::string &&action, std::vector<Identifier> &&ids)
      : action(std::move(action)), ids(std::move(ids)) {}

  bool operator==(const equippable &other) const {
    return action == other.action && ids == other.ids;
  }

  bool operator!=(const equippable &other) const { return !(*this == other); }

  Interactable::Item in_inventory() const {
    auto inventory = Inventory::GetItems();
    for (const auto &item : inventory) {
      if constexpr (std::is_same_v<Identifier, std::string> ||
                    std::is_same_v<Identifier, std::string_view>) {
        if (std::find(ids.begin(), ids.end(), item.GetName()) != ids.end())
          return item;
      } else if constexpr (std::is_same_v<Identifier, std::int32_t>) {
        if (std::find(ids.begin(), ids.end(), item.GetID()) != ids.end())
          return item;
      }
    }
    return Interactable::Item();
  }

  Interactable::Item in_equipment() const {
    auto inventory = Equipment::GetItems();
    for (const auto &item : inventory) {
      if constexpr (std::is_same_v<Identifier, std::string> ||
                    std::is_same_v<Identifier, std::string_view>) {
        if (std::find(ids.begin(), ids.end(), item.GetName()) != ids.end())
          return item;
      } else if constexpr (std::is_same_v<Identifier, std::int32_t>) {
        if (std::find(ids.begin(), ids.end(), item.GetID()) != ids.end())
          return item;
      }
    }
    return Interactable::Item();
  }

  bool equip() const {
    auto item = in_inventory();
    if (!item)
      return in_equipment();

    if (!item.Interact(action))
      return equip();

    return WaitFunc(1250, 50, [&]() { return in_equipment(); });
  }
};

template <typename Identifier> class gear_set {
public:
  using array_type = std::array<std::optional<equippable<Identifier>>, 11>;

  using iterator = array_type::iterator;

  using const_iterator = array_type::const_iterator;

  gear_set() = default;

  gear_set(const gear_set &other) : _gear(other._gear) {}

  gear_set(gear_set &&other) : _gear(std::move(other._gear)) {}

  gear_set &operator=(const gear_set &other) {
    _gear = other._gear;
    return *this;
  }

  gear_set &operator=(gear_set &&other) {
    _gear = std::move(other._gear);
    return *this;
  }

  bool operator==(const gear_set &other) const { return _gear == other._gear; }

  bool operator!=(const gear_set &other) const { return !(*this == other); }

  iterator begin() { return _gear.begin(); }

  iterator end() { return _gear.end(); }

  const_iterator begin() const { return _gear.begin(); }

  const_iterator end() const { return _gear.end(); }

  equippable<Identifier> &operator[](Equipment::SLOT slot) {
    return _gear[static_cast<std::size_t>(slot)];
  }

  const equippable<Identifier> &operator[](Equipment::SLOT slot) const {
    return _gear[static_cast<std::size_t>(slot)];
  }

  equippable<Identifier> &get(Equipment::SLOT slot) {
    return _gear[static_cast<std::size_t>(slot)];
  }

  const equippable<Identifier> &get(Equipment::SLOT slot) const {
    return _gear[static_cast<std::size_t>(slot)];
  }

  void set(Equipment::SLOT slot, const equippable<Identifier> &equip) {
    _gear[static_cast<std::size_t>(slot)] = equip;
  }

  void set(Equipment::SLOT slot, equippable<Identifier> &&equip) {
    _gear[static_cast<std::size_t>(slot)] = std::move(equip);
  }

  template <typename... Args>
  void emplace(Equipment::SLOT slot, const std::string &action,
               const Args &...ids) {
    _gear[static_cast<std::size_t>(slot)] =
        equippable<Identifier>(action, {ids...});
  }

  bool equipped() {
    for (const auto &equip : _gear) {
      if (!equip.has_value())
        continue;

      if (!equip.value().in_equipment()) {
        return false;
      }
    }
    return true;
  }

  bool equip() {
    for (const auto &equip : _gear) {
      if (!equip.has_value())
        continue;

      if (!equip.value().equip()) {
        return false;
      }
    }

    return equipped();
  }

  bool has() {
    for (const auto &equip : _gear) {
      if (!equip.has_value())
        continue;

      if (!equip.value().in_inventory() && !equip.value().in_equipment()) {
        return false;
      }
    }
    return true;
  }

private:
  array_type _gear;
};

} // namespace as
