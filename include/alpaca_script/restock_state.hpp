#pragma once

#include <alpaca_script/script_context.hpp>
#include <alpaca_script/web_walker/banks/banks.hpp>
#include <alpaca_script/web_walker/web_walker.hpp>

namespace as {
template <typename Context>
class restock_state : public as::script_state<Context> {
public:
  void handle(Context &ctx) override {
    const auto &ge = as::web_walker::banks::grand_exchange;
    const auto pos = Minimap::GetPosition();

    if (!ge.in_area(pos)) {
      const auto path = as::web_walker::find_path(pos, ge.location);
      if (!as::web_walker::walk_path(path, 6))
        return;
    }
  }
};
} // namespace as