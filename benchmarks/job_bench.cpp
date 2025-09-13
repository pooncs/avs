#include <avs/runtime/job_system.hpp>
#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std::chrono;

int main(){
    avs::runtime::JobSystem js({std::thread::hardware_concurrency()});
    js.start();

    const int N = 1'000'000;
    std::vector<int> data(N, 1);

    auto t0 = high_resolution_clock::now();
    std::atomic<long long> sum{0};
    js.parallel_for<int>(0, N, 1024, [&](int i){ sum.fetch_add(data[i], std::memory_order_relaxed); });
    auto t1 = high_resolution_clock::now();

    auto dt_ms = duration_cast<milliseconds>(t1-t0).count();
    std::cout << "parallel_for sum=" << sum.load() << " time_ms=" << dt_ms << "\n";

    // Enqueue micro tasks
    const int Tasks = 500'000;
    std::atomic<int> done{0};
    auto t2 = high_resolution_clock::now();
    for (int i=0;i<Tasks;++i) js.submit([&]{ done.fetch_add(1, std::memory_order_relaxed); });
    // Spin wait using a small sleep
    while (done.load(std::memory_order_acquire) != Tasks) std::this_thread::yield();
    auto t3 = high_resolution_clock::now();
    auto dt2 = duration_cast<milliseconds>(t3-t2).count();
    std::cout << "submit throughput tasks/s=" << (Tasks*1000ll)/(dt2?dt2:1) << " time_ms=" << dt2 << "\n";

    js.stop();
    return 0;
}
