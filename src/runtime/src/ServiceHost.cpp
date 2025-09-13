#include "avs/runtime/ServiceHost.h"
#include "avs/runtime/HeadlessExecutor.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <winsvc.h>
#include <string>
#include <atomic>

namespace {
SERVICE_STATUS_HANDLE g_statusHandle = nullptr;
std::atomic<bool> g_stop{false};
avs::runtime::HeadlessExecutor* g_exec = nullptr;
avs::runtime::HeadlessConfig g_cfg;

void ReportStatus(DWORD state) {
    SERVICE_STATUS status{};
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = state;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    SetServiceStatus(g_statusHandle, &status);
}

void WINAPI ServiceCtrlHandler(DWORD ctrl) {
    if (ctrl == SERVICE_CONTROL_STOP || ctrl == SERVICE_CONTROL_SHUTDOWN) {
        ReportStatus(SERVICE_STOP_PENDING);
        g_stop.store(true);
        if (g_exec) g_exec->stop();
        ReportStatus(SERVICE_STOPPED);
    }
}

void WINAPI ServiceMain(DWORD, LPTSTR*) {
    g_statusHandle = RegisterServiceCtrlHandler(L"AVS_Headless", ServiceCtrlHandler);
    if (!g_statusHandle) return;
    ReportStatus(SERVICE_START_PENDING);

    std::string err;
    if (!g_exec->start(g_cfg, err)) {
        ReportStatus(SERVICE_STOPPED);
        return;
    }

    ReportStatus(SERVICE_RUNNING);
    while (!g_stop.load()) Sleep(200);
}
} // anon

#endif // _WIN32

namespace avs { namespace runtime { namespace service {

bool Install(const std::wstring& name, const std::wstring& desc, const std::wstring& exePath) {
#ifdef _WIN32
    SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
    if (!scm) return false;
    SC_HANDLE svc = CreateServiceW(
        scm, name.c_str(), name.c_str(), SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, exePath.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr);
    if (!svc) { CloseServiceHandle(scm); return false; }
    SERVICE_DESCRIPTIONW sd{ const_cast<LPWSTR>(desc.c_str()) };
    ChangeServiceConfig2W(svc, SERVICE_CONFIG_DESCRIPTION, &sd);
    CloseServiceHandle(svc); CloseServiceHandle(scm);
    return true;
#else
    (void)name; (void)desc; (void)exePath; return false;
#endif
}

bool Uninstall(const std::wstring& name) {
#ifdef _WIN32
    SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scm) return false;
    SC_HANDLE svc = OpenServiceW(scm, name.c_str(), DELETE);
    if (!svc) { CloseServiceHandle(scm); return false; }
    bool ok = DeleteService(svc) != 0;
    CloseServiceHandle(svc); CloseServiceHandle(scm);
    return ok;
#else
    (void)name; return false;
#endif
}

int Run(const std::wstring& name, HeadlessExecutor& exec, const HeadlessConfig& cfg) {
#ifdef _WIN32
    g_exec = &exec; g_cfg = cfg;
    SERVICE_TABLE_ENTRYW table[] = { { const_cast<LPWSTR>(name.c_str()), ServiceMain }, { nullptr, nullptr } };
    if (!StartServiceCtrlDispatcherW(table)) return 1;
    return 0;
#else
    (void)name; (void)exec; (void)cfg; return 1;
#endif
}

}}} // namespace avs::runtime::service
