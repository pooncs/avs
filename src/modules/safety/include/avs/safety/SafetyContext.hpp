
#pragma once
#include "avs/safety/SafetyManager.hpp"

namespace avs { namespace safety {

// Global access to a process-level SafetyManager.
struct SafetyContext {
  static SafetyManager& instance() {
    static SafetyManager mgr;
    return mgr;
  }
};

}} // namespace avs::safety
