#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <functional>

// Simple metrics: counter, gauge, timer. Export snapshot for GUI.

namespace avs { namespace core {

struct Counter { std::atomic<long long> value{0}; void inc(long long v=1){ value.fetch_add(v, std::memory_order_relaxed);} };
struct Gauge   { std::atomic<double> value{0.0}; void set(double v){ value.store(v, std::memory_order_relaxed);} };
struct Timer   {
    std::atomic<long long> count{0};
    std::atomic<long long> total_ns{0};
    void observe_ns(long long ns){ count.fetch_add(1, std::memory_order_relaxed); total_ns.fetch_add(ns, std::memory_order_relaxed);} 
    double avg_ms() const { long long c=count.load(); return c? (total_ns.load()/1e6)/double(c) : 0.0; }
};

class TelemetryRegistry {
public:
    static TelemetryRegistry& instance();

    Counter& counter(const std::string& name);
    Gauge&   gauge  (const std::string& name);
    Timer&   timer  (const std::string& name);

    struct MetricValue { std::string name; std::string kind; double v1; double v2; long long i1; };
    std::vector<MetricValue> snapshot();

private:
    TelemetryRegistry() = default;
    TelemetryRegistry(const TelemetryRegistry&) = delete; TelemetryRegistry& operator=(const TelemetryRegistry&) = delete;
    std::mutex mtx_;
    std::unordered_map<std::string, Counter> counters_;
    std::unordered_map<std::string, Gauge> gauges_;
    std::unordered_map<std::string, Timer> timers_;
};

// Scoped timer helper.
class ScopedTimer {
public:
    explicit ScopedTimer(Timer& t): t_(t), start_(std::chrono::high_resolution_clock::now()){}
    ~ScopedTimer(){ auto end=std::chrono::high_resolution_clock::now(); auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start_).count(); t_.observe_ns(ns);} 
private:
    Timer& t_;
    std::chrono::high_resolution_clock::time_point start_;
};

}} // namespace
