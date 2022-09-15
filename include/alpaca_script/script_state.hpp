#pragma once

#include <memory>

namespace as {
class script_context_base;

class script_state_base {
public:
  virtual void handle(script_context_base &ctx) = 0;

  virtual ~script_state_base() = default;
};

template <typename Context = script_context_base> class script_state;

template <> class script_state<script_context_base> : public script_state_base {
public:
  virtual void handle(script_context_base &ctx) = 0;
};

template <typename Context> class script_state : public script_state_base {
public:
  virtual void handle(script_context_base &ctx) override {
    handle(static_cast<Context &>(ctx));
  }

  virtual void handle(Context &ctx) = 0;
};
} // namespace as