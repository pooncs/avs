
#pragma once
#include <string>
namespace avs { namespace scripting {
struct PyConfig { std::string program_name; };
class Python {
public:
    static bool init(const PyConfig&);
    static bool run(const std::string& code);
    static void shutdown();
};
}} // namespace
