#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <optional>
#include <condition_variable>
#include <mutex>
#include <utility>
#include <type_traits>

// Public API
namespace avs { namespace runtime {

class JobSystem {
public:
    using JobFn = std::function<void()>;

    struct Config {
        unsigned worker_count = std::thread::hardware_concurrency();
        // Optional thread affinity or names can be added later.
    };

    JobSystem();
    explicit JobSystem(const Config& cfg);
    ~JobSystem();

    JobSystem(const JobSystem&) = delete;
    JobSystem& operator=(const JobSystem&) = delete;

    void start();
    void stop();

    // Submit a job; safe from any thread.
    void submit(JobFn job);

    // Parallel for [begin, end) with given grain.
    template<typename Index, typename F>
    void parallel_for(Index begin, Index end, Index grain, F&& fn) {
        static_assert(std::is_integral<Index>::value, "Index must be integral");
        if (end <= begin) return;
        if (grain <= 0) grain = 1;

        struct Block { Index b, e; F* f; JobSystem* js; std::shared_ptr<std::atomic<int>> counter; };
        auto blocks = std::make_shared<std::vector<Block>>();
        auto counter = std::make_shared<std::atomic<int>>(0);
        auto n = (end - begin + grain - 1) / grain;
        blocks->reserve(static_cast<size_t>(n));
        for (Index b = begin; b < end; b += grain) {
            Index e = std::min<Index>(b + grain, end);
            blocks->push_back(Block{b, e, &fn, this, counter});
        }
        counter->store((int)blocks->size(), std::memory_order_relaxed);

        for (auto &blk : *blocks) {
            submit([blk]() mutable {
                for (Index i = blk.b; i < blk.e; ++i) (*blk.f)(i);
                blk.counter->fetch_sub(1, std::memory_order_acq_rel);
            });
        }
        // Wait until all blocks done.
        wait_until_zero(counter);
    }

    // Returns number of worker threads.
    unsigned workers() const noexcept { return (unsigned)workers_.size(); }

private:
    struct Worker;
    struct TLS;

    void worker_loop(size_t idx);
    void wait_until_zero(const std::shared_ptr<std::atomic<int>>& counter);

    std::vector<std::unique_ptr<Worker>> workers_;
    std::vector<std::thread> threads_;

    std::atomic<bool> running_{false};

    // Global submission queue (MPMC).
    struct MPMCQueue;
    std::unique_ptr<MPMCQueue> global_;

    // Sleep/wake.
    std::mutex cv_mtx_;
    std::condition_variable cv_;
    std::atomic<int> sleeping_{0};

    Config cfg_{};
};

}} // namespace avs::runtime
