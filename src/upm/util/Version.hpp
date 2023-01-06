#pragma once

#include <regex>
#include <string>
namespace upm {

// TODO: harden for other versioning systems.
// May need to declare unique version comparators for certain packages,
// but that assumes they fall outside any logical structure.
//
// Problem for later though
class Version {
private:
    static inline std::regex COMPONENT_SPLIT = std::regex("[.-]");

    std::string version;
    std::vector<std::string> components;

public:
    /**
     * Initialize the version from the provided string.
     * Note that due to conventions, if the version starts with a (lower-case) v, the character
     * is stripped
     */
    Version(const std::string& version);

    const std::string& getVersion();
    const std::string& getVersion() const { return version; }

    /**
     * Checks whether the same component in two different version objects are the same.
     * Note that the interpretation of this must be checked by the consumer. The only check performed is on a single
     * component level.
     */
    bool componentEquals(const Version& b, size_t idx);

    friend bool operator<(const Version& a, const Version& b);
    friend bool operator>(const Version& a, const Version& b);
    friend bool operator==(const Version& a, const Version& b);
    friend bool operator==(const Version& a, const std::string& b);
};

bool operator<(const Version& a, const Version& b);
bool operator>(const Version& a, const Version& b);
bool operator==(const Version& a, const Version& b);
bool operator==(const Version& a, const std::string& b);
bool operator!=(const Version& a, const Version& b);

}
