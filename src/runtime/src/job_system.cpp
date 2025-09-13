#include "avs/runtime/job_system.hpp"
#include "avs/runtime/work_stealing_deque.hpp"
#include "avs/runtime/lockfree_mpmc_queue.hpp"
#include <random>
#include <chrono>

namespace avs { namespace runtime {

struct JobSystem::MPMCQueue {
    MPMCRing<JobFn> q{1<<16};
    bool push(JobFn j){ return q.enqueue(j);} 
    bool pop(JobFn &j){ return q.dequeue(j);} 
};

struct JobSystem::Worker {
    WorkStealingDeque<JobFn> deq{1024};
};

JobSystem::JobSystem() : JobSystem(Config{}) {}
JobSystem::JobSystem(const Config& cfg) : cfg_(cfg) {}
JobSystem::~JobSystem(){ stop(); }

void JobSystem::start(){
    if (running_.exchange(true)) return;
    global_ = std::make_unique<MPMCQueue>();
    size_t n = cfg_.worker_count ? cfg_.worker_count : 1;
    workers_.reserve(n);
    for (size_t i=0;i<n;++i) workers_.push_back(std::make_unique<Worker>());
    threads_.reserve(n);
    for (size_t i=0;i<n;++i){
        threads_.emplace_back([this,i]{ worker_loop(i); });
    }
}

void JobSystem::stop(){
    if (!running_.exchange(false)) return;
    {
        std::lock_guard<std::mutex> lk(cv_mtx_);
        cv_.notify_all();
    }
    for (auto &t : threads_) if (t.joinable()) t.join();
    threads_.clear();
    workers_.clear();
    global_.reset();
}

void JobSystem::submit(JobFn job){
    // Try local push if called from worker thread?
    // For simplicity, push to global.
    while (!global_->push(std::move(job))){ std::this_thread::yield(); }
    // Wake one sleeper
    if (sleeping_.load(std::memory_order_relaxed) > 0){
        std::lock_guard<std::mutex> lk(cv_mtx_);
        cv_.notify_one();
    }
}

void JobSystem::worker_loop(size_t idx){
    std::mt19937 rng((uint32_t)(std::chrono::high_resolution_clock::now().time_since_epoch().count()+idx));
    std::uniform_int_distribution<size_t> dist(0, workers_.size() ? workers_.size()-1 : 0);

    JobFn job;
    while (running_.load(std::memory_order_relaxed)){
        bool did = false;
        // 1) Own deque
        if (workers_[idx]->deq.pop_bottom(job)) { job(); did = true; }
        // 2) Global
        else if (global_->pop(job)) { job(); did = true; }
        else {
            // 3) Steal
            for (int s=0;s<4 && running_.load(std::memory_order_relaxed);++s){
                size_t v = dist(rng);
                if (v==idx) continue;
                if (workers_[v]->deq.steal_top(job)){ job(); did = true; break; }
            }
        }
        if (!did){
            // Sleep a bit
            sleeping_.fetch_add(1, std::memory_order_relaxed);
            std::unique_lock<std::mutex> lk(cv_mtx_);
            cv_.wait_for(lk, std::chrono::milliseconds(1));
            sleeping_.fetch_sub(1, std::memory_order_relaxed);
        }
    }
}

void JobSystem::wait_until_zero(const std::shared_ptr<std::atomic<int>>& counter){
    using namespace std::chrono_literals;
    while (counter->load(std::memory_order_acquire) > 0){
        // Help run jobs while waiting
        JobFn job;
        if (global_ && global_->pop(job)) job(); else std::this_thread::sleep_for(100us);
    }
}

}} // namespace
