
#pragma once
#include <string>
#include <chrono>
#include "avs/safety/SafetyState.hpp"

namespace avs { namespace safety {

class ISafetyBackend {
public:
  virtual ~ISafetyBackend() = default;

  // Init/shutdown backend. Return false on error.
  virtual bool initialize() = 0;
  virtual void shutdown() = 0;

  // Read combined safety state (E-stop + faults).
  virtual SafetyState read_state() = 0;

  // Send heartbeat (best-effort).
  virtual void send_heartbeat() = 0;

  // Request reset/acknowledge safety circuit if applicable.
  virtual bool request_reset() = 0;

  // Introspection
  virtual const char* name() const noexcept = 0;
  virtual std::string last_error() const = 0;
};

}} // namespace avs::safety
