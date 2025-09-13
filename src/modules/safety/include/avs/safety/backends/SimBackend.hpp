
#pragma once
#include <atomic>
#include <mutex>
#include "avs/safety/ISafetyBackend.hpp"

namespace avs { namespace safety { namespace backends {

class SimSafetyBackend : public ISafetyBackend {
public:
  SimSafetyBackend() = default;
  ~SimSafetyBackend() override = default;

  bool initialize() override { return true; }
  void shutdown() override {}

  SafetyState read_state() override {
    std::lock_guard<std::mutex> lk(mu_);
    return state_;
  }

  void send_heartbeat() override {}

  bool request_reset() override {
    std::lock_guard<std::mutex> lk(mu_);
    if (state_ == SafetyState::EStopEngaged || state_ == SafetyState::Fault) {
      state_ = SafetyState::Ok; // simple simulation
      return true;
    }
    return false;
  }

  const char* name() const noexcept override { return "SimSafetyBackend"; }
  std::string last_error() const override { return last_err_; }

  // Simulation controls
  void force_state(SafetyState s) {
    std::lock_guard<std::mutex> lk(mu_);
    state_ = s;
  }

private:
  mutable std::mutex mu_{};
  SafetyState state_{SafetyState::Ok};
  std::string last_err_{};
};

}}} // namespace avs::safety::backends
