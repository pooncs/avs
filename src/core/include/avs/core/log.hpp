
    #pragma once
    #include <iostream>
    #include <mutex>

    namespace avs::core {
    inline std::mutex& log_mutex() { static std::mutex m; return m; }
    template<typename... Args>
    inline void log(const char* level, Args&&... args) {
      std::lock_guard<std::mutex> lk(log_mutex());
      (std::cout << '[' << level << "] ", ... , (std::cout << args)) << '
';
    }
    } // namespace avs::core
