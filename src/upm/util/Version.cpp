#include "Version.hpp"

#include <string>
#include <iostream>

namespace upm {

Version::Version(const std::string& version) : version(version) {
    std::sregex_token_iterator it(version.begin(), version.end(), COMPONENT_SPLIT, -1), end;
    while (it != end) {
        components.push_back(*it);
        it++;
    }
}

const std::string& Version::getVersion() {
    return this->version;
}

bool operator<(const Version& a, const Version& b) {
    for (size_t i = 0; i < std::max(a.components.size(), b.components.size()); ++i) {
        auto ca = i < a.components.size() ? a.components.at(i) : "0";
        auto cb = i < b.components.size() ? b.components.at(i) : "0";
        if (ca.size() < cb.size() || ca.compare(cb) < 0) {
            return true;
        }
    }
    return false;
}
bool operator==(const Version &a, const Version &b) {
    return a.version == b.version || (!(a < b) && !(a > b));
}

bool operator!=(const Version &a, const Version &b) {
    return !(a == b);
}

bool operator>(const Version &a, const Version &b) {
    for (size_t i = 0; i < std::max(a.components.size(), b.components.size()); ++i) {
        auto ca = i < a.components.size() ? a.components.at(i) : "0";
        auto cb = i < b.components.size() ? b.components.at(i) : "0";
        if (ca.size() > cb.size() || ca.compare(cb) > 0) {
            return true;
        }
    }
    return false;
}

}
