#include "Version.hpp"
#include "stc/StringUtil.hpp"

#include <string>
#include <iostream>

namespace upm {

Version::Version(const std::string& rawVersion) : version(rawVersion.at(0) == 'v' ? rawVersion.substr(1) : rawVersion) {
    std::sregex_token_iterator it(version.begin(), version.end(), COMPONENT_SPLIT, -1);
    std::sregex_token_iterator end;
    while (it != end) {
        components.push_back(*it);
        it++;
    }
}

const std::string& Version::getVersion() {
    return this->version;
}

bool Version::componentEquals(const Version& b, size_t idx) {
    // TODO: cache in the class itself
    std::vector<std::string>
        aComp = stc::string::split(version, ".");
    std::vector<std::string>
        bComp = stc::string::split(b.version, ".");

    if (idx >= aComp.size()) {
        throw std::runtime_error("Illegal version access; has " + std::to_string(aComp.size()) + " components, required component " + std::to_string(idx));
    } else if (aComp.size() != bComp.size()) {
        return false;
    }
    return aComp.at(idx) == bComp.at(idx);
}

bool operator<(const Version& a, const Version& b) {
    return b > a;
}
bool operator==(const Version &a, const Version &b) {
    return a.version == b.version;
}

bool operator==(const Version& a, const std::string& b) {
    return a.version == b;
}

bool operator!=(const Version &a, const Version &b) {
    return !(a == b);
}

bool operator>(const Version& a, const Version& b) {
    std::vector<std::string>
        aComp = stc::string::split(a.getVersion(), ".");
    std::vector<std::string>
        bComp = stc::string::split(b.getVersion(), ".");

    for (int i = 0; i < std::max(aComp.size(), bComp.size()); ++i) {
        auto aBit = i >= aComp.size() ? 0 : std::stoi(aComp[i]);
        auto bBit = i >= bComp.size() ? 0 : std::stoi(bComp[i]);

        if (aBit > bBit) {
            return true;
        } else if (aBit < bBit) { return false;
}
    }
    return false;
}

}
