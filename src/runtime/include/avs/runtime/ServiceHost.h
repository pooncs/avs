#pragma once
#include <string>
#include <functional>

namespace avs { namespace runtime {

class HeadlessExecutor;
struct HeadlessConfig;

// Windows service wrapper (no-op on non-Windows)
namespace service {

bool Install(const std::wstring& name, const std::wstring& desc, const std::wstring& exePath);
bool Uninstall(const std::wstring& name);
int  Run(const std::wstring& name, HeadlessExecutor& exec, const HeadlessConfig& cfg);

} // namespace service

}} // namespace avs::runtime
