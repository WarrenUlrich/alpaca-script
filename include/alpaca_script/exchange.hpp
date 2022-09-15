#pragma once

#include <Game/Core.hpp>
#include <string>
#include <vector>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib/httplib.h>
#include <nlohmann/json.hpp>

#include "mouse_camera.hpp"

namespace as::exchange {

enum class slot : std::int32_t {
  first = 7,
  second = 8,
  third = 9,
  fourth = 10,
  fifth = 11,
  sixth = 12,
  seventh = 13,
  eighth = 14,
};

enum class slot_state : std::int32_t {
  empty,
  sell,
  buy,
};

namespace detail {
constexpr auto root_widget_id = 465;

std::array<Interactable::Widget, 8> get_slot_root_widgets() {
  std::array<Interactable::Widget, 8> widgets;
  for (int i = static_cast<std::int32_t>(slot::first);
       i <= static_cast<std::int32_t>(slot::eighth); i++) {
    widgets[i - static_cast<std::int32_t>(slot::first)] =
        Widgets::Get(root_widget_id, i);
  }

  return widgets;
}

Interactable::Widget get_slot_root_widget(slot s) {
  return Widgets::Get(root_widget_id, static_cast<std::int32_t>(s));
}

Interactable::Widget get_slot_buy_offer_widget(slot s) {
  return Interactable::Widget(get_slot_root_widget(s).GetChildren(3));
}

Interactable::Widget get_slot_sell_offer_widget(slot s) {
  return Interactable::Widget(get_slot_root_widget(s).GetChildren(4));
}

Interactable::Widget get_slot_status_widget(slot s) {
  return Interactable::Widget(get_slot_root_widget(s).GetChildren(22));
}

std::string get_slot_item_name(slot s) {
  return get_slot_root_widget(s).GetChildren(19).GetText();
}

std::string get_buy_offer_text_input() {
  const auto widget = Widgets::Get(162, 42);
  const auto text = widget.GetText();

  const auto begin = text.find("</col>") + 6;
  if (begin == std::string::npos || begin >= text.size())
    return "";

  const auto end = text.find("*", begin + 1);

  return text.substr(begin, end - begin);
}

Interactable::Widget get_buy_offer_first_search_result() {
  return Interactable::Widget(Widgets::Get(162, 50, 1));
}

std::string get_buy_offer_search_result_name(
    const Interactable::Widget &widget = get_buy_offer_first_search_result()) {
  return widget.GetText();
}

Interactable::Widget get_offer_enter_quantity_widget() {
  return Interactable::Widget(Widgets::Get(root_widget_id, 25, 49));
}

Interactable::Widget get_offer_enter_price_widget() {
  return Interactable::Widget(Widgets::Get(root_widget_id, 25, 52));
}

Interactable::Widget get_confirm_offer_widget() {
  return Interactable::Widget(Widgets::Get(root_widget_id, 25, 54));
}

Interactable::Widget get_offer_back_button_widget() {
  return Interactable::Widget(Widgets::Get(root_widget_id, 4));
}

Interactable::Widget collect_all_button() {
  return Interactable::Widget(Widgets::Get(root_widget_id, 6, 1));
}

std::string get_offer_name() {
  const auto widget = Widgets::Get(465, 25, 25);
  return widget.GetText();
}

std::int32_t get_offer_quantity() {
  const auto widget = Widgets::Get(465, 25, 32);
  if (!widget)
    return 0;

  auto text = widget.GetText();
  if (text.empty())
    return 0;

  text.erase(std::remove(text.begin(), text.end(), ','), text.end());
  return std::stoi(text);
}

std::int32_t get_offer_price_per_item() {
  const auto widget = Widgets::Get(465, 25, 39);
  if (!widget)
    return 0;

  auto text = widget.GetText();
  if (text.empty())
    return 0;

  auto num_end = std::find(text.begin(), text.end(), ' ');

  if (num_end == text.end())
    return 0;

  text.erase(std::remove(text.begin(), num_end, ','), text.end());
  return std::stoi(text);
}
} // namespace detail

class order {
public:
  std::int32_t item_id;
  std::int32_t quantity;
  std::int32_t price_per_item;

  order(std::int32_t item_id, std::int32_t quantity,
        std::int32_t price_per_item)
      : item_id(item_id), quantity(quantity), price_per_item(price_per_item) {}

  std::string item_name() const {
    auto item_info = Internal::GetItemInfo(item_id);
    if (!item_info) {
      item_info = Internal::LoadItemInfo(item_id);
      if (!item_info)
        throw std::runtime_error("Failed to load item info");
    }

    return item_info.GetName();
  }

  std::int32_t total_price() const { return quantity * price_per_item; }
};

bool is_open() {
  const auto widget = Widgets::Get(detail::root_widget_id, 2, 1);
  if (!widget) {
    return false;
  }

  if (widget.IsHidden() || widget.GetHidden() || !widget.IsVisible())
    return false;

  return true;
  // return widget.GetText().find("Grand Exchange") != std::string::npos;
}

bool open() {
  const auto npc = NPCs::Get("Grand Exchange Clerk");
  if (!npc)
    return false;

  if (!npc.GetVisibility() < .60)
    as::mouse_camera::rotate_to(npc.GetTile(), 20);

  if (!npc.Interact("Exchange Grand Exchange Clerk"))
    return false;

  return WaitFunc(2500, 50, is_open);
}

bool close() {
  if (!is_open())
    return true;

  const auto widget = Widgets::Get(detail::root_widget_id, 2, 11);
  if (!widget)
    return false;

  if (!widget.Interact("Close"))
    return false;

  return WaitFunc(2500, 50, [] { return !is_open(); });
}

bool in_slot_selection() {
  const auto widget = Widgets::Get(465, 6, 2);
  if (!widget)
    return false;

  if (!widget.IsVisible())
    return false;

  return widget.GetText().find("Select an offer slot") != std::string::npos;
}

slot_state get_slot_state(slot s) {
  const auto root = detail::get_slot_root_widget(s);
  if (!root)
    throw std::runtime_error("slot root widget is null");

  const auto header = root.GetChildren(16);
  if (!header)
    throw std::runtime_error("slot header widget is null");

  const auto text = header.GetText();
  if (text.find("Empty") != std::string::npos)
    return slot_state::empty;
  else if (text.find("Sell") != std::string::npos)
    return slot_state::sell;
  else if (text.find("Buy") != std::string::npos)
    return slot_state::buy;
  else
    throw std::runtime_error("unknown slot state");
}

double get_slot_status(slot s) {
  const auto widget = detail::get_slot_status_widget(s);
  if (!widget)
    throw std::runtime_error("slot status widget is null");

  if (widget.GetHidden() || widget.IsHidden())
    return 0.0;

  const auto width = widget.GetWidth();
  return static_cast<double>(width) / 105.0;
}

bool is_slot_complete(slot s) { return get_slot_status(s) == 1.0; }

bool is_buy_offer_open() {
  const auto widget = Widgets::Get(detail::root_widget_id, 25, 18);

  if (!widget) {
    return false;
  }

  if (widget.IsHidden() || widget.GetHidden() || !widget.IsVisible())
    return false;

  const auto text = widget.GetText();
  return text.find("Buy offer") != std::string::npos;
}

bool is_sell_offer_open() {
  const auto widget = Widgets::Get(detail::root_widget_id, 25, 18);

  if (!widget) {
    return false;
  }

  if (widget.IsHidden() || widget.GetHidden() || !widget.IsVisible())
    return false;

  const auto text = widget.GetText();
  return text.find("Sell offer") != std::string::npos;
}

bool can_collect_all() {
  const auto widget = detail::collect_all_button();
  if (!widget)
    return false;

  return widget.IsVisible();
}

bool collect_to_inventory() {
  const auto widget = detail::collect_all_button();
  if (!widget)
    return false;

  if (!widget.IsVisible())
    return false;

  if (!widget.Interact("Collect to inventory"))
    return false;

  if (!WaitFunc(2000, 50,
                []() { return !detail::collect_all_button().IsVisible(); }))
    return false;

  Wait(2500); // Slot widgets take forever to update after collecting
  return true;
}

bool collect_to_bank() {
  const auto widget = detail::collect_all_button();
  if (!widget)
    return false;

  if (!widget.IsVisible())
    return false;

  if (!widget.Interact("Collect to bank"))
    return false;

  if (!WaitFunc(2000, 50,
                []() { return !detail::collect_all_button().IsVisible(); }))
    return false;

  Wait(2500); // Slot widgets take forever to update after collecting
  return true;
}

bool set_buy_offer_name(const std::string &name) {
  if (!is_buy_offer_open())
    return false;

  auto input = detail::get_buy_offer_text_input();
  while (input.size() > 1) {
    Debug::Info << "Backspace" << std::endl;
    Interact::TypeKey(Key::KEY_BACKSPACE);
    input = detail::get_buy_offer_text_input();
  }

  if (!Interact::TypeString(name))
    return false;

  Interactable::Widget first_result;

  if (!WaitFunc(2000, 50, [&]() {
        first_result = detail::get_buy_offer_first_search_result();
        auto first_result_name =
            detail::get_buy_offer_search_result_name(first_result);
        return first_result_name.find(name) != std::string::npos;
      })) {
    Debug::Error << "Failed to find search result" << std::endl;
    return false;
  }

  if (!first_result.Interact())
    return false;

  return WaitFunc(2000, 50, [&]() { return detail::get_offer_name() == name; });
}

bool set_sell_offer_item(const std::int32_t id) {
  auto item = Inventory::GetItem(id);
  std::string name;

  if (!item) {
    const auto item_info = Internal::GetItemInfo(id);
    if (!item_info)
      return false;

    name = item_info.GetName();
    item = Inventory::GetItem(name);
    if (!item)
      return false;
  }

  if (!item.Interact("Offer"))
    return false;

  return WaitFunc(2000, 50, [&]() { return detail::get_offer_name() == name; });
}

bool set_offer_quantity(std::int32_t amount) {
  if (detail::get_offer_quantity() == amount)
    return true;

  const auto widget = detail::get_offer_enter_quantity_widget();

  if (!widget)
    return false;

  if (!widget.Interact())
    return false;

  if (!WaitFunc(2000, 50, []() {
        return Chat::GetDialogueState() == Chat::ENTER_AMOUNT;
      }))
    return false;

  if (!Chat::EnterAmount(amount))
    return false;

  return WaitFunc(2000, 50,
                  [&]() { return detail::get_offer_quantity() == amount; });
}

bool set_offer_price_per_item(std::int32_t price) {
  const auto widget = detail::get_offer_enter_price_widget();

  if (!widget)
    return false;

  if (!widget.Interact())
    return false;

  if (!WaitFunc(2000, 50, []() {
        return Chat::GetDialogueState() == Chat::ENTER_AMOUNT;
      }))
    return false;

  if (!Chat::EnterAmount(price))
    return false;

  return WaitFunc(
      2000, 50, [&]() { return detail::get_offer_price_per_item() == price; });
}

// bool confirm_offer(slot s, const std::string &name) {
//   const auto widget = detail::get_confirm_offer_widget();

//   if (!widget)
//     return false;

//   if (!widget.Interact())
//     return false;

//   if (!WaitFunc(2000, 50, in_slot_selection))
//     return false;

//   return WaitFunc(2000, 50,
//                   [&]() { return detail::get_slot_item_name(s) == name; });
// }

bool confirm_offer() {
  const auto widget = detail::get_confirm_offer_widget();

  if (!widget)
    return false;

  if (!widget.Interact())
    return false;

  return WaitFunc(2000, 50, in_slot_selection);
}

bool backout_offer() {
  const auto widget = detail::get_offer_back_button_widget();

  if (!widget)
    return false;

  if (!widget.Interact())
    return false;

  return WaitFunc(2000, 50, [&]() { return in_slot_selection(); });
}

bool open_buy_offer(slot s) {
  if (!in_slot_selection())
    return false;

  const auto widget = detail::get_slot_buy_offer_widget(s);
  if (!widget)
    throw std::runtime_error("buy offer widget is null");

  if (!widget.Interact())
    return false;

  return WaitFunc(2000, 50, []() {
    return Chat::GetDialogueState() == Chat::ENTER_AMOUNT &&
           is_buy_offer_open();
  });
}

bool open_sell_offer(slot s) {
  if (!in_slot_selection())
    return false;

  const auto widget = detail::get_slot_sell_offer_widget(s);
  if (!widget)
    throw std::runtime_error("sell offer widget is null");

  if (!widget.Interact())
    return false;

  return WaitFunc(2000, 50, is_sell_offer_open);
}

bool create_buy_offer(slot s, const order &o) {
  if (!is_open())
    return false;

  if (is_buy_offer_open())
    if (!backout_offer())
      return false;

  if (get_slot_state(s) != slot_state::empty)
    return false;

  if (!open_buy_offer(s))
    return false;

  if (!set_buy_offer_name(o.item_name()))
    return false;

  if (!set_offer_quantity(o.quantity))
    return false;

  if (!set_offer_price_per_item(o.price_per_item))
    return false;

  // return confirm_offer(s, name);
  return confirm_offer();
}

bool create_sell_offer(slot s, const order &o) {
  if (!is_open())
    return false;

  if (is_sell_offer_open())
    if (!backout_offer())
      return false;

  if (get_slot_state(s) != slot_state::empty)
    return false;

  if (!open_sell_offer(s))
    return false;

  if (!set_sell_offer_item(o.item_id))
    return false;

  if (!set_offer_quantity(o.quantity))
    return false;

  if (!set_offer_price_per_item(o.price_per_item))
    return false;

  // return confirm_offer(s, name);
  return confirm_offer();
}

std::int32_t get_wiki_price(std::int32_t item_id) {
  auto cli = httplib::Client("https://prices.runescape.wiki");

  std::string request_url = std::string("/api/v1/osrs/latest?id=");
  request_url += std::to_string(item_id);

  const auto res = cli.Get(request_url);

  if (!res)
    throw std::runtime_error("failed to get wiki price");

  if (res->status != 200)
    throw std::runtime_error("failed to get wiki price");

  const auto json = nlohmann::json::parse(res->body);
  const auto high =
      json["data"][std::to_string(item_id)]["high"].get<std::int32_t>();
  const auto low =
      json["data"][std::to_string(item_id)]["low"].get<std::int32_t>();
  return (high + low) / 2;
}
} // namespace as::exchange