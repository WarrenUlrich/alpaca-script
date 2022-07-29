#pragma once

#include <Game/Core.hpp>
#include <optional>
#include <unordered_map>
#include <vector>

namespace as {

template <typename ItemIdentifier> class equipment_set {
public:
  using mapped_type = std::vector<ItemIdentifier>;

  using key_type = Equipment::SLOT;

  using map_type = std::unordered_map<key_type, mapped_type>;

  using value_type = typename map_type::value_type;

  using iterator = typename map_type::iterator;

  using const_iterator = typename map_type::const_iterator;

  equipment_set() = default;

  equipment_set(const equipment_set &other) : _map(other._map) {}

  equipment_set(equipment_set &&other) : _map(std::move(other._map)) {}

  equipment_set(const map_type &map) : _map(map) {}

  equipment_set(map_type &&map) : _map(std::move(map)) {}

  equipment_set &operator=(const equipment_set &other) {
    _map = other._map;
    return *this;
  }

  equipment_set &operator=(equipment_set &&other) {
    _map = std::move(other._map);
    return *this;
  }

  equipment_set &operator=(const map_type &map) {
    _map = map;
    return *this;
  }

  equipment_set &operator=(map_type &&map) {
    _map = std::move(map);
    return *this;
  }

  const mapped_type &operator[](const key_type &key) const {
    return _map.at(key);
  }

  mapped_type &operator[](const key_type &key) { return _map[key]; }

  mapped_type &at(const key_type &key) { return _map.at(key); }

  const mapped_type &at(const key_type &key) const { return _map.at(key); }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args &&...args) {
    return _map.emplace(std::forward<Args>(args)...);
  }

  std::pair<iterator, bool> insert(const value_type &value) {
    return _map.insert(value);
  }

  std::pair<iterator, bool> insert(value_type &&value) {
    return _map.insert(std::move(value));
  }

  iterator begin() { return _map.begin(); }

  const_iterator begin() const { return _map.begin(); }

  iterator end() { return _map.end(); }

  const_iterator end() const { return _map.end(); }

  bool empty() const { return _map.empty(); }

  bool contains(const key_type &key) const {
    return _map.find(key) != _map.end();
  }

  void clear() { _map.clear(); }

  void erase(const key_type &key) { _map.erase(key); }

  void swap(equipment_set &other) { _map.swap(other._map); }

  size_t size() const { return _map.size(); }

  bool equipped(const key_type &key) const {
    if (!contains(key)) {
      return false;
    }

    const auto &ids = at(key);
    auto item = Equipment::GetItem(key);
    if (!item) {
      return false;
    }

    if constexpr (std::is_same_v<ItemIdentifier, std::string>) {
      for (const auto &id : ids) {
        if (item.GetName() == id) {
          return true;
        }
      }

      return false;
    } else if constexpr (std::is_same_v<ItemIdentifier, std::uint32_t>) {
      for (const auto &id : ids) {
        if (item.GetID() == id) {
          return true;
        }
      }
      return false;
    }
    throw std::runtime_error("Invalid ItemIdentifier type");
  }

  bool equipped() const {
    for (const auto &[key, _] : _map) {
      if (!equipped(key)) {
        return false;
      }
    }

    return true;
  }

private:
  map_type _map;
};

template <typename Key, typename ItemIdentifier = std::string>
class equipment_manager {
public:
  using mapped_type = equipment_set<ItemIdentifier>;

  using key_type = Key;

  using map_type = std::unordered_map<key_type, mapped_type>;

  using iterator = map_type::iterator;

  using const_iterator = map_type::const_iterator;

  equipment_manager() = default;

  equipment_manager(const equipment_manager &other) noexcept
      : _map(other._map) {}

  equipment_manager(equipment_manager &&other) noexcept
      : _map(std::move(other._map)) {}

  equipment_manager(const map_type &map) noexcept : _map(map) {}

  equipment_manager(map_type &&map) noexcept : _map(std::move(map)) {}

  iterator begin() noexcept { return _map.begin(); }

  iterator end() noexcept { return _map.end(); }

  const_iterator begin() const noexcept { return _map.begin(); }

  const_iterator end() const noexcept { return _map.end(); }

  mapped_type &at(const key_type &key) { return _map.at(key); }

  const mapped_type &at(const key_type &key) const { return _map.at(key); }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args &&...args) {
    return _map.emplace(std::forward<Args>(args)...);
  }

  iterator find(const key_type &key) { return _map.find(key); }

  const_iterator find(const key_type &key) const { return _map.find(key); }

  bool contains(const key_type &key) const {
    return _map.find(key) != _map.end();
  }

private:
  map_type _map;
};
} // namespace as
