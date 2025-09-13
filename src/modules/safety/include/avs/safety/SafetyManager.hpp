
#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include "avs/safety/ISafetyBackend.hpp"

namespace avs { namespace safety {

enum class Mode { Simulation, Hardware };

class SafetyManager {
public:
  using StateCallback = std::function<void(SafetyState)>;

  SafetyManager();
  ~SafetyManager();

  SafetyManager(const SafetyManager&) = delete;
  SafetyManager& operator=(const SafetyManager&) = delete;

  // Configure
  void set_backend(std::unique_ptr<ISafetyBackend> backend); // call when stopped
  void set_mode(Mode m);                                     // call when stopped
  void set_heartbeat_period(std::chrono::milliseconds p);
  void set_poll_period(std::chrono::milliseconds p);

  // Control
  bool start();
  void stop();

  // Query
  SafetyState current_state() const noexcept { return state_.load(); }
  bool estop_engaged() const noexcept { return current_state() == SafetyState::EStopEngaged; }
  bool ok_to_run() const noexcept { return current_state() == SafetyState::Ok; }
  Mode mode() const noexcept { return mode_; }

  // Actions
  bool request_reset();

  // Events
  void on_state_change(StateCallback cb);

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  std::atomic<SafetyState> state_{SafetyState::Unknown};
  std::atomic<bool> running_{false};
  std::thread worker_{};
  std::mutex mu_;

  Mode mode_{Mode::Simulation};
  std::unique_ptr<ISafetyBackend> backend_{}; // set by user; if null in Simulation, we create Sim backend
  std::chrono::milliseconds heartbeat_period_{100};
  std::chrono::milliseconds poll_period_{20};

  StateCallback on_change_{};

  void run_();
  void update_state_(SafetyState s);
};

}} // namespace avs::safety
