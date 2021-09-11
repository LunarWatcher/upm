#include "StrUtil.hpp"

namespace upm {

std::vector<std::string> StrUtil::splitString(std::string input, const std::string& delimiter, int limit) {
    // Special case: no delimiter.
    if (delimiter.length() == 0 || limit == 0)
        return {input};

    std::vector<std::string> out;
    size_t pos = 0;
    std::string token;
    int count = 0;
    while ((pos = input.find(delimiter)) != std::string::npos) {
        token = input.substr(0, pos);
        out.push_back(token);
        input.erase(0, pos + delimiter.length());
        count++;
        if (count == limit) {
            break;
        }
    }
    out.push_back(input);
    return out;
}

}
