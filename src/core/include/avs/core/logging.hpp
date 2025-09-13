#pragma once
#include <string>
#include <string_view>
#include <mutex>
#include <fstream>
#include <chrono>
#include <atomic>
#include <sstream>
#include <thread>
#include <vector>
#include <map>

// AVS core logging: structured JSON lines + rotating files.
// C++17, RAII, header-only API with pImpl impl in logging.cpp
// Keep comments brief.

namespace avs { namespace core {

enum class LogLevel { Trace=0, Debug, Info, Warn, Error, Critical };

struct LogConfig {
    bool to_console = true;
    bool to_file = false;
    std::string file_path;        // e.g., logs/avs.log
    std::size_t max_bytes = 10 * 1024 * 1024; // 10MB
    int max_files = 5;            // N rotated files kept
    LogLevel level = LogLevel::Info;
};

class Logger {
public:
    static Logger& instance();

    // Init once; safe to call multiple times.
    void init(const LogConfig& cfg);

    void set_level(LogLevel lvl);
    LogLevel level() const noexcept;

    // Structured log with key-value fields.
    void log(LogLevel lvl, std::string_view msg,
             const std::map<std::string, std::string>& fields = {},
             const char* file = "", int line = 0);

    // Convenience
    void trace(std::string_view m, const std::map<std::string,std::string>& f = {}, const char* file = "", int line = 0);
    void debug(std::string_view m, const std::map<std::string,std::string>& f = {}, const char* file = "", int line = 0);
    void info (std::string_view m, const std::map<std::string,std::string>& f = {}, const char* file = "", int line = 0);
    void warn (std::string_view m, const std::map<std::string,std::string>& f = {}, const char* file = "", int line = 0);
    void error(std::string_view m, const std::map<std::string,std::string>& f = {}, const char* file = "", int line = 0);
    void critical(std::string_view m, const std::map<std::string,std::string>& f = {}, const char* file = "", int line = 0);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete; Logger& operator=(const Logger&) = delete;

    struct Impl; // pImpl
    Impl* impl_;
};

// Macros capture file/line.
#define AVS_LOG_TRACE(msg, fields) ::avs::core::Logger::instance().trace((msg), (fields), __FILE__, __LINE__)
#define AVS_LOG_DEBUG(msg, fields) ::avs::core::Logger::instance().debug((msg), (fields), __FILE__, __LINE__)
#define AVS_LOG_INFO(msg, fields)  ::avs::core::Logger::instance().info ((msg), (fields), __FILE__, __LINE__)
#define AVS_LOG_WARN(msg, fields)  ::avs::core::Logger::instance().warn ((msg), (fields), __FILE__, __LINE__)
#define AVS_LOG_ERROR(msg, fields) ::avs::core::Logger::instance().error((msg), (fields), __FILE__, __LINE__)
#define AVS_LOG_CRIT(msg, fields)  ::avs::core::Logger::instance().critical((msg), (fields), __FILE__, __LINE__)

// Init from env (optional). AVS_LOG_PATH, AVS_LOG_MAX_SIZE, AVS_LOG_MAX_FILES, AVS_LOG_LEVEL
void init_default_logging_from_env();

}} // namespace
