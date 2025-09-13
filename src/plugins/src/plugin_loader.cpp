#include <avs/plugin/plugin_loader.hpp>

#include <unordered_set>
#include <mutex>
#include <iostream>
#ifdef _WIN32
  #define NOMINMAX
  #include <windows.h>
#endif

using namespace std;

namespace avs::plugin {

namespace {

#ifdef _WIN32
struct Library {
    HMODULE h{nullptr};
    explicit Library(HMODULE m=nullptr) : h(m) {}
    Library(const Library&) = delete; Library& operator=(const Library&) = delete;
    Library(Library&& o) noexcept : h(o.h) { o.h = nullptr; }
    Library& operator=(Library&& o) noexcept { if (this!=&o){ close(); h=o.h; o.h=nullptr;} return *this; }
    ~Library(){ close(); }
    void close(){ if(h){ FreeLibrary(h); h=nullptr; } }
    bool valid() const { return h!=nullptr; }
};
#endif

struct PluginModule {
    std::filesystem::path path;
#ifdef _WIN32
    Library lib;
#endif
    PluginInfo info{ };
};

std::mutex g_mu;
std::vector<PluginModule>& modules() {
    static std::vector<PluginModule> m; return m;
}
std::unordered_set<std::wstring>& seen() {
    static std::unordered_set<std::wstring> s; return s;
}

bool has_ext_case_insensitive(const std::filesystem::path& p, const std::wstring& ext) {
    auto e = p.extension().wstring();
    if (e.size() != ext.size()) return false;
    for (size_t i=0;i<e.size();++i) {
        wchar_t a = towlower(e[i]);
        wchar_t b = towlower(ext[i]);
        if (a!=b) return false;
    }
    return true;
}

} // namespace

std::vector<std::filesystem::path> currently_loaded() {
    std::scoped_lock lk(g_mu);
    std::vector<std::filesystem::path> out;
    out.reserve(modules().size());
    for (auto& m : modules()) out.push_back(m.path);
    return out;
}

std::vector<LoadedPlugin> load_all_from_dir(const std::filesystem::path& dir) {
    std::vector<LoadedPlugin> out;
#ifdef _WIN32
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) return out;

    for (auto& entry : std::filesystem::directory_iterator(dir)) {
        const auto& p = entry.path();
        if (!entry.is_regular_file()) continue;
        if (!has_ext_case_insensitive(p, L".dll")) continue;

        std::wstring wpath = p.wstring();
        {
            std::scoped_lock lk(g_mu);
            if (seen().count(wpath)) continue; // already loaded
        }

        HMODULE mod = LoadLibraryW(wpath.c_str());
        if (!mod) {
            std::wcerr << L"[AVS][plugin] Failed to load: " << wpath << L"
";
            continue;
        }

        using GetInfoFn = avs::plugin::PluginInfo (__cdecl*)();
        using RegisterFn = int (__cdecl*)();

        auto get_info = reinterpret_cast<GetInfoFn>(GetProcAddress(mod, "avs_plugin_get_info"));
        auto reg      = reinterpret_cast<RegisterFn>(GetProcAddress(mod, "avs_plugin_register"));
        if (!get_info || !reg) {
            std::wcerr << L"[AVS][plugin] Missing required exports in: " << wpath << L"
";
            FreeLibrary(mod);
            continue;
        }

        PluginInfo info = get_info();
        if (info.abi_version != avs::core::PLUGIN_ABI_VERSION) {
            std::wcerr << L"[AVS][plugin] ABI mismatch for: " << wpath
                       << L" (plugin abi=" << info.abi_version << L", host=" << avs::core::PLUGIN_ABI_VERSION << L")
";
            FreeLibrary(mod);
            continue;
        }

        int rc = reg();
        if (rc != 0) {
            std::wcerr << L"[AVS][plugin] Registration failed for: " << wpath << L" (rc=" << rc << L")
";
            FreeLibrary(mod);
            continue;
        }

        PluginModule m; m.path = p; m.lib = Library(mod); m.info = info;
        {
            std::scoped_lock lk(g_mu);
            seen().insert(wpath);
            modules().push_back(std::move(m));
        }
        out.push_back(LoadedPlugin{p, info});

        std::wcerr << L"[AVS][plugin] Loaded: " << wpath << L" name=" << info.name << L" ver=" << info.version << L"
";
    }
#endif
    return out;
}

static vector<filesystem::path> split_paths_env(std::wstring s) {
    vector<filesystem::path> out;
#ifdef _WIN32
    const wchar_t sep = L';';
#else
    const wchar_t sep = L':';
#endif
    wstring cur;
    for (wchar_t c : s) {
        if (c==sep) { if (!cur.empty()) { out.emplace_back(cur); cur.clear(); } }
        else cur.push_back(c);
    }
    if (!cur.empty()) out.emplace_back(cur);
    return out;
}

std::vector<LoadedPlugin> bootstrap_from_env_or_default(const std::filesystem::path& exe_dir) {
    vector<LoadedPlugin> out;
#ifdef _WIN32
    wchar_t buf[4096]; DWORD n = GetEnvironmentVariableW(L"AVS_PLUGIN_DIR", buf, 4096);
    if (n>0 && n<4096) {
        for (auto& p : split_paths_env(buf)) {
            auto v = load_all_from_dir(p);
            out.insert(out.end(), v.begin(), v.end());
        }
    } else {
        auto def = exe_dir / "plugins";
        auto v = load_all_from_dir(def);
        out.insert(out.end(), v.begin(), v.end());
    }
#endif
    return out;
}

} // namespace avs::plugin
