#pragma once

#include <memory>

namespace as {
class script_context;

class script_state_base {
public:
  virtual void handle(script_context &ctx) = 0;

  virtual ~script_state_base() = default;
};

template <typename Context = script_context> class script_state;

template <> class script_state<script_context> : public script_state_base {
public:
  virtual void handle(script_context &ctx) = 0;
};

template <typename Context> class script_state : public script_state_base {
public:
  virtual void handle(script_context &ctx) override {
    handle(static_cast<Context &>(ctx));
  }

  virtual void handle(Context &ctx) = 0;
};

class script_context {
public:
  bool finished;
  std::unique_ptr<script_state_base> state;

  script_context() : finished(false), state(nullptr) {}

  script_context(std::unique_ptr<script_state_base> &&state)
      : finished(false), state(std::move(state)) {}

  script_context(script_context &ctx) = delete;

  script_context(script_context &&other) : state(std::move(other.state)) {}

  script_context &operator=(script_context &ctx) = delete;

  script_context &operator=(script_context &&other) {
    state = std::move(other.state);
    return *this;
  }

  void handle() {
    if (state) {
      state->handle(*this);
    }
  }

  template <typename State, typename... Args> void set_state(Args &&...args) {
    state = std::make_unique<State>(std::forward<Args>(args)...);
  }

  // template <typename State> void set_state(State &&state) {
  //   this->state = std::make_unique<State>(std::move(state));
  // }
};
} // namespace as