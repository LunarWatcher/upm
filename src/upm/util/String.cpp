#include "String.hpp"

#include <sstream>

namespace upm {


std::string String::escapeShellArg(const std::string& raw) {
    std::stringstream ss;
    ss << "'";

    for (const auto& ch : raw) {
        if (ch == '\'') {
            ss << "'\\''";
        } else {
            ss << ch;
        }
    }

    ss << "'";
    return ss.str();
}

}
