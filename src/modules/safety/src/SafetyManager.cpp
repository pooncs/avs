
#include "avs/safety/SafetyManager.hpp"
#include "avs/safety/backends/SimBackend.hpp"
#include <cassert>

using namespace std::chrono;

namespace avs { namespace safety {

struct SafetyManager::Impl {
  // Placeholder for future heavy impl (I/O handles, etc.)
};

SafetyManager::SafetyManager() : impl_(std::make_unique<Impl>()) {}
SafetyManager::~SafetyManager() { stop(); }

void SafetyManager::set_backend(std::unique_ptr<ISafetyBackend> backend) {
  std::lock_guard<std::mutex> lk(mu_);
  assert(!running_.load());
  backend_ = std::move(backend);
}

void SafetyManager::set_mode(Mode m) {
  std::lock_guard<std::mutex> lk(mu_);
  assert(!running_.load());
  mode_ = m;
}

void SafetyManager::set_heartbeat_period(milliseconds p) { heartbeat_period_ = p; }
void SafetyManager::set_poll_period(milliseconds p) { poll_period_ = p; }

bool SafetyManager::start() {
  std::lock_guard<std::mutex> lk(mu_);
  if (running_.load()) return true;

  if (!backend_) {
    if (mode_ == Mode::Simulation) {
      backend_ = std::make_unique<backends::SimSafetyBackend>();
    }
  }

  if (!backend_) return false;
  if (!backend_->initialize()) return false;

  running_.store(true);
  worker_ = std::thread(&SafetyManager::run_, this);
  return true;
}

void SafetyManager::stop() {
  if (!running_.exchange(false)) return;
  if (worker_.joinable()) worker_.join();
  std::lock_guard<std::mutex> lk(mu_);
  if (backend_) backend_->shutdown();
}

bool SafetyManager::request_reset() {
  std::lock_guard<std::mutex> lk(mu_);
  if (!backend_) return false;
  return backend_->request_reset();
}

void SafetyManager::on_state_change(StateCallback cb) {
  std::lock_guard<std::mutex> lk(mu_);
  on_change_ = std::move(cb);
}

void SafetyManager::update_state_(SafetyState s) {
  auto prev = state_.exchange(s);
  if (on_change_ && prev != s) {
    try { on_change_(s); } catch (...) { /* no-throw */ }
  }
}

void SafetyManager::run_() {
  auto next_hb = steady_clock::now();
  auto next_poll = steady_clock::now();

  while (running_.load()) {
    auto now = steady_clock::now();
    if (now >= next_poll) {
      SafetyState s = SafetyState::Unknown;
      {
        std::lock_guard<std::mutex> lk(mu_);
        if (backend_) s = backend_->read_state();
      }
      update_state_(s);
      next_poll = now + poll_period_;
    }
    if (now >= next_hb) {
      std::lock_guard<std::mutex> lk(mu_);
      if (backend_) backend_->send_heartbeat();
      next_hb = now + heartbeat_period_;
    }
    std::this_thread::sleep_for(1ms);
  }
}

}} // namespace avs::safety
