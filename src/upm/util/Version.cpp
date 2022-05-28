#include "Version.hpp"
#include "upm/util/StrUtil.hpp"

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
    return b > a;
}
bool operator==(const Version &a, const Version &b) {
    return a.version == b.version || (!(a < b) && !(a > b));
}

bool operator==(const Version& a, const std::string& b) {
    return a.version == b;
}

bool operator!=(const Version &a, const Version &b) {
    return !(a == b);
}

bool operator>(const Version& a, const Version& b) {
    std::vector<std::string>
        aComp = StrUtil::splitString(a.getVersion(), "."),
        bComp = StrUtil::splitString(b.getVersion(), ".");

    for (int i = 0; i < std::max(aComp.size(), bComp.size()); ++i) {
        auto aBit = i >= aComp.size() ? 0 : std::stoi(aComp[i]);
        auto bBit = i >= bComp.size() ? 0 : std::stoi(bComp[i]);

        if (aBit > bBit) {
            return true;
        } else if (aBit < bBit) return false;
    }
    return false;
}

}
