#include "avs/core/logging.hpp"
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

namespace avs { namespace core {

static const char* lvl_str(LogLevel l){
    switch(l){
        case LogLevel::Trace: return "trace";
        case LogLevel::Debug: return "debug";
        case LogLevel::Info: return "info";
        case LogLevel::Warn: return "warn";
        case LogLevel::Error: return "error";
        case LogLevel::Critical: return "critical";
    }
    return "info";
}

struct Logger::Impl {
    std::mutex mtx;
    LogConfig cfg;
    std::ofstream ofs;
    std::atomic<bool> inited{false};

    void ensure_open(){
        if(cfg.to_file && !ofs.is_open()){
            fs::create_directories(fs::path(cfg.file_path).parent_path());
            ofs.open(cfg.file_path, std::ios::out | std::ios::app);
        }
    }

    std::string now_iso8601() {
        using clock = std::chrono::system_clock;
        auto tp = clock::now();
        auto t = clock::to_time_t(tp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;
        std::tm tmv{};
#ifdef _WIN32
        localtime_s(&tmv, &t);
#else
        localtime_r(&t, &tmv);
#endif
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.%03d",
            tmv.tm_year+1900, tmv.tm_mon+1, tmv.tm_mday, tmv.tm_hour, tmv.tm_min, tmv.tm_sec, (int)ms.count());
        return std::string(buf);
    }

    void rotate_if_needed(){
        if(!cfg.to_file) return;
        try{
            if(!fs::exists(cfg.file_path)) return;
            auto sz = fs::file_size(cfg.file_path);
            if(sz < cfg.max_bytes) return;
            // rotate: avs.log -> avs.1.log ... up to max_files-1
            for(int i=cfg.max_files-1; i>=1; --i){
                auto src = cfg.file_path + "." + std::to_string(i);
                auto dst = cfg.file_path + "." + std::to_string(i+1);
                if(fs::exists(dst)) fs::remove(dst);
                if(fs::exists(src)) fs::rename(src, dst);
            }
            auto first = cfg.file_path + ".1";
            if(ofs.is_open()) ofs.close();
            if(fs::exists(first)) fs::remove(first);
            fs::rename(cfg.file_path, first);
            // reopen
            ofs.open(cfg.file_path, std::ios::out | std::ios::trunc);
        } catch(...) {
            // best effort
        }
    }

    void write_line(const std::string& line){
        if(cfg.to_console){
            std::fwrite(line.data(), 1, line.size(), stdout);
            std::fwrite("\n", 1, 1, stdout);
            std::fflush(stdout);
        }
        if(cfg.to_file){
            ensure_open();
            if(ofs.is_open()){
                ofs << line << '\n';
                ofs.flush();
                rotate_if_needed();
            }
        }
    }
};

Logger::Logger(): impl_(new Impl){}
Logger::~Logger(){
    delete impl_;
}

Logger& Logger::instance(){ static Logger g; return g; }

void Logger::init(const LogConfig& c){
    std::lock_guard<std::mutex> lock(impl_->mtx);
    impl_->cfg = c;
    impl_->inited = true;
    if(impl_->cfg.to_file){ impl_->ensure_open(); }
}

void Logger::set_level(LogLevel lvl){ std::lock_guard<std::mutex> lock(impl_->mtx); impl_->cfg.level = lvl; }
LogLevel Logger::level() const noexcept { return impl_->cfg.level; }

void Logger::log(LogLevel lvl, std::string_view msg, const std::map<std::string, std::string>& fields, const char* file, int line){
    if((int)lvl < (int)impl_->cfg.level) return;
    std::lock_guard<std::mutex> lock(impl_->mtx);
    std::ostringstream os;
    os << '{';
    os << "\"ts\":\"" << impl_->now_iso8601() << "\",";
    os << "\"lvl\":\"" << lvl_str(lvl) << "\",";
    os << "\"thr\":" << std::this_thread::get_id() << ",";
    os << "\"msg\":\"";
    // escape quotes in msg
    for(char c: msg){ if(c=='"') os << "\\\""; else if(c=='\\') os << "\\\\"; else if(c=='\n') os << "\\n"; else os << c; }
    os << "\",";
    os << "\"file\":\"" << file << "\",";
    os << "\"line\":" << line << ",";
    os << "\"fields\":{";
    bool first=true;
    for(const auto& kv: fields){
        if(!first) os << ','; first=false;
        os << "\"" << kv.first << "\":\"";
        for(char c: kv.second){ if(c=='"') os << "\\\""; else if(c=='\\') os << "\\\\"; else if(c=='\n') os << "\\n"; else os << c; }
        os << "\"";
    }
    os << "}}";
    impl_->write_line(os.str());
}

void Logger::trace(std::string_view m, const std::map<std::string,std::string>& f, const char* file, int line){ log(LogLevel::Trace,m,f,file,line);} 
void Logger::debug(std::string_view m, const std::map<std::string,std::string>& f, const char* file, int line){ log(LogLevel::Debug,m,f,file,line);} 
void Logger::info (std::string_view m, const std::map<std::string,std::string>& f, const char* file, int line){ log(LogLevel::Info ,m,f,file,line);} 
void Logger::warn (std::string_view m, const std::map<std::string,std::string>& f, const char* file, int line){ log(LogLevel::Warn ,m,f,file,line);} 
void Logger::error(std::string_view m, const std::map<std::string,std::string>& f, const char* file, int line){ log(LogLevel::Error,m,f,file,line);} 
void Logger::critical(std::string_view m, const std::map<std::string,std::string>& f, const char* file, int line){ log(LogLevel::Critical,m,f,file,line);} 

static LogLevel parse_level(const char* s){
    if(!s) return LogLevel::Info;
    std::string v(s); for(auto& c: v) c= (char)tolower(c);
    if(v=="trace") return LogLevel::Trace;
    if(v=="debug") return LogLevel::Debug;
    if(v=="info") return LogLevel::Info;
    if(v=="warn"||v=="warning") return LogLevel::Warn;
    if(v=="error") return LogLevel::Error;
    if(v=="critical"||v=="crit") return LogLevel::Critical;
    return LogLevel::Info;
}

void init_default_logging_from_env(){
    LogConfig cfg; 
    const char* path = std::getenv("AVS_LOG_PATH");
    if(path){ cfg.to_file = true; cfg.file_path = path; cfg.to_console = std::getenv("AVS_LOG_NO_CONSOLE")? false : true; }
    const char* lvl = std::getenv("AVS_LOG_LEVEL");
    cfg.level = parse_level(lvl);
    if(const char* ms = std::getenv("AVS_LOG_MAX_SIZE")) cfg.max_bytes = std::strtoull(ms,nullptr,10);
    if(const char* mf = std::getenv("AVS_LOG_MAX_FILES")) cfg.max_files = std::atoi(mf);
    Logger::instance().init(cfg);
    Logger::instance().info("logging initialized", {{"to_file", cfg.to_file?"1":"0"}, {"file", cfg.file_path}});
}

}} // namespace
