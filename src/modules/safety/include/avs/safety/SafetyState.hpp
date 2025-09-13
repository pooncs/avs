
#pragma once
#include <cstdint>

namespace avs { namespace safety {

enum class SafetyState : uint8_t {
  Unknown = 0,
  Ok,
  EStopEngaged,
  Fault
};

inline const char* to_string(SafetyState s) noexcept {
  switch (s) {
    case SafetyState::Ok: return "Ok";
    case SafetyState::EStopEngaged: return "EStopEngaged";
    case SafetyState::Fault: return "Fault";
    default: return "Unknown";
  }
}

}} // namespace avs::safety
