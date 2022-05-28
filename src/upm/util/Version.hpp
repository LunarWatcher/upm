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
    Version(const std::string& version);

    const std::string& getVersion();
    const std::string& getVersion() const { return version; }

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
