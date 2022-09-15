#pragma once

#include <Game/Core.hpp>

#include "script_state.hpp"

namespace as {
class script_context_base {
public:
  bool finished = false;

  std::unique_ptr<script_state_base> state;

  script_context_base() = default;

  script_context_base(const script_context_base &other) = delete;

  script_context_base(script_context_base &&other)
      : finished(other.finished), state(std::move(other.state)) {}

  script_context_base(std::unique_ptr<script_state_base> &&state)
      : state(std::move(state)) {}

  virtual void handle() = 0;

  template <typename State> void set_state() {
    state = std::make_unique<State>();
  }

  template <typename State, typename... Args> void set_state(Args &&...args) {
    state = std::make_unique<State>(std::forward<Args>(args)...);
  }

  ~script_context_base() = default;
};

template <typename Context> class script_context : public script_context_base {
public:
  class default_login_state : public script_state<Context> {
  public:
    void handle(Context &ctx) override {
      if (Mainscreen::IsLoggedIn()) {
        ctx.state = std::make_unique<typename Context::on_start_state>();
        return;
      }

      if (!Login::LoginPlayer()) {
        Debug::Fatal << "LoginPlayer failed, stopping." << std::endl;
        ctx.finished = true;
        return;
      }

      ctx.state = std::make_unique<typename Context::on_start_state>();
    }
  };

  script_context()
      : script_context_base(
            std::make_unique<typename Context::on_start_state>()) {}

  script_context(std::unique_ptr<script_state_base> &&state)
      : script_context_base(std::move(state)) {}

  script_context(script_context &ctx) = delete;

  script_context(script_context &&other)
      : script_context_base(std::move(other)) {}

  script_context &operator=(script_context &ctx) = delete;

  script_context &operator=(script_context &&other) {
    state = std::move(other.state);
    return *this;
  }

  void handle() override {
    if (!Mainscreen::IsLoggedIn()) {
      if constexpr (requires { typename Context::login_state; }) {
        state =
            std::make_unique<typename Context::login_state>(std::move(state));
      } else {
        state = std::make_unique<default_login_state>();
      }
    }

    if (state) {
      state->handle(*this);
    } else {
      state = std::make_unique<typename Context::on_start_state>();
      state->handle(*this);
    }
  }
};
} // namespace as