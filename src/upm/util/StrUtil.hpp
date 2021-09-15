#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

namespace upm {
namespace StrUtil {
std::vector<std::string> splitString(std::string input, const std::string& delimiter, int limit = -1);
}
}
