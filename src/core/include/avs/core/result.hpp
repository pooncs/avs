
#pragma once
#include <string>

namespace avs::core {
struct Result {
  bool ok{true};
  std::string msg{};
  static Result success() { return {true, {}}; }
  static Result failure(std::string m) { return {false, std::move(m)}; }
  explicit operator bool() const { return ok; }
};
} // namespace avs::core
