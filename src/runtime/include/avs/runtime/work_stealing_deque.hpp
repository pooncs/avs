#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace avs { namespace runtime {

// Chase-Lev work-stealing deque for single producer (owner) and multiple thieves.
// Simplified and bounded power-of-two capacity, dynamic grow on owner side.
// Comments minimal by request.

template<typename T>
class WorkStealingDeque {
public:
    WorkStealingDeque(size_t capacity = 1024) {
        capacity_ = 1;
        while (capacity_ < capacity) capacity_ <<= 1;
        buffer_.reset(new Slot[capacity_]);
        top_.store(0, std::memory_order_relaxed);
        bottom_.store(0, std::memory_order_relaxed);
    }

    ~WorkStealingDeque() = default;

    bool push_bottom(const T& v) {
        auto b = bottom_.load(std::memory_order_relaxed);
        auto t = top_.load(std::memory_order_acquire);
        if ((b - t) >= static_cast<long>(capacity_ - 1)) {
            // Grow
            grow(); b = bottom_.load(std::memory_order_relaxed); t = top_.load(std::memory_order_acquire);
            if ((b - t) >= static_cast<long>(capacity_ - 1)) return false;
        }
        buffer_[b & mask()].val = v;
        std::atomic_thread_fence(std::memory_order_release);
        bottom_.store(b + 1, std::memory_order_relaxed);
        return true;
    }

    bool pop_bottom(T& out) {
        auto b = bottom_.load(std::memory_order_relaxed) - 1;
        bottom_.store(b, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto t = top_.load(std::memory_order_relaxed);
        if (t <= b) {
            out = std::move(buffer_[b & mask()].val);
            if (t == b) {
                if (!top_.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed)) {
                    // Lost race
                    out = T{}; // drop
                    return false;
                }
                bottom_.store(b + 1, std::memory_order_relaxed);
            }
            return true;
        } else {
            bottom_.store(b + 1, std::memory_order_relaxed);
            return false;
        }
    }

    bool steal_top(T& out) {
        auto t = top_.load(std::memory_order_acquire);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto b = bottom_.load(std::memory_order_acquire);
        if (t < b) {
            out = std::move(buffer_[t & mask()].val);
            if (!top_.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed)) {
                return false;
            }
            return true;
        }
        return false;
    }

private:
    struct Slot { T val; };

    void grow() {
        size_t newcap = capacity_ << 1;
        std::unique_ptr<Slot[]> nb(new Slot[newcap]);
        auto t = top_.load(std::memory_order_acquire);
        auto b = bottom_.load(std::memory_order_acquire);
        for (long i = t; i < b; ++i) {
            nb[i & (newcap - 1)].val = std::move(buffer_[i & mask()].val);
        }
        buffer_.swap(nb);
        capacity_ = newcap;
    }

    size_t mask() const { return capacity_ - 1; }

    std::unique_ptr<Slot[]> buffer_{};
    size_t capacity_{};
    std::atomic<long> top_{};
    std::atomic<long> bottom_{};
};

}} // namespace avs::runtime
