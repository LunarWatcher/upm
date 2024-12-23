#pragma once

#include <string>
#include <vector>

namespace upm::String {

extern std::string escapeShellArg(const std::string& raw);
extern std::string command(const std::vector<std::string>& parts);

}
