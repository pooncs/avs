#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <type_traits>

namespace avs { namespace runtime {

template<typename T>
class MPMCRing {
public:
    explicit MPMCRing(size_t capacity = 1<<16) : cap_(round_pow2(capacity)), mask_(cap_-1) {
        cells_.reset(new Cell[cap_]);
        for (size_t i=0;i<cap_;++i) cells_[i].seq.store(i, std::memory_order_relaxed);
        head_.store(0, std::memory_order_relaxed);
        tail_.store(0, std::memory_order_relaxed);
    }

    bool enqueue(const T& v) {
        Cell* c;
        size_t pos = head_.load(std::memory_order_relaxed);
        for (;;) {
            c = &cells_[pos & mask_];
            size_t seq = c->seq.load(std::memory_order_acquire);
            intptr_t dif = (intptr_t)seq - (intptr_t)pos;
            if (dif == 0) {
                if (head_.compare_exchange_weak(pos, pos+1, std::memory_order_relaxed)) break;
            } else if (dif < 0) {
                return false;
            } else {
                pos = head_.load(std::memory_order_relaxed);
            }
        }
        c->val = v;
        c->seq.store(pos+1, std::memory_order_release);
        return true;
    }

    bool dequeue(T& v) {
        Cell* c;
        size_t pos = tail_.load(std::memory_order_relaxed);
        for (;;) {
            c = &cells_[pos & mask_];
            size_t seq = c->seq.load(std::memory_order_acquire);
            intptr_t dif = (intptr_t)seq - (intptr_t)(pos+1);
            if (dif == 0) {
                if (tail_.compare_exchange_weak(pos, pos+1, std::memory_order_relaxed)) break;
            } else if (dif < 0) {
                return false;
            } else {
                pos = tail_.load(std::memory_order_relaxed);
            }
        }
        v = std::move(c->val);
        c->seq.store(pos + mask_ + 1, std::memory_order_release);
        return true;
    }

private:
    struct Cell { std::atomic<size_t> seq; T val; };

    static size_t round_pow2(size_t v){ size_t p=1; while(p<v) p<<=1; return p;}

    std::unique_ptr<Cell[]> cells_{};
    size_t cap_{};
    size_t mask_{};
    std::atomic<size_t> head_{};
    std::atomic<size_t> tail_{};
};

}} // namespace avs::runtime
