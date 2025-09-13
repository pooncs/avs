
#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace avs::util {
class ThreadPool {
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::mutex m_;
  std::condition_variable cv_;
  std::atomic<bool> stop_{false};
public:
  explicit ThreadPool(unsigned n = std::thread::hardware_concurrency()) {
    if (n == 0) n = 1;
    workers_.reserve(n);
    for (unsigned i=0; i<n; ++i) {
      workers_.emplace_back([this]{
        for(;;){
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lk(m_);
            cv_.wait(lk, [this]{ return stop_ || !tasks_.empty(); });
            if (stop_ && tasks_.empty()) return;
            task = std::move(tasks_.front());
            tasks_.pop();
          }
          task();
        }
      });
    }
  }
  ~ThreadPool(){
    {
      std::lock_guard<std::mutex> lk(m_);
      stop_ = true;
    }
    cv_.notify_all();
    for(auto& t: workers_) if (t.joinable()) t.join();
  }
  void submit(std::function<void()> f){
    {
      std::lock_guard<std::mutex> lk(m_);
      tasks_.push(std::move(f));
    }
    cv_.notify_one();
  }
  unsigned size() const { return (unsigned)workers_.size(); }
};
} // namespace avs::util
