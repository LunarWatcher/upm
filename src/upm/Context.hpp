#pragma once

#include "upm/conf/Config.hpp"
#include "upm/util/Version.hpp"
#include <vector>
#include <string>


namespace upm {

enum class VersionType {
    AT,
    APPROX,
    IMPLICIT
};

class Context {
private:
    std::vector<std::string> input;
public:
    std::string package;
    std::string packageVersion;
    VersionType versionType;

    const bool isRoot;
    Config cfg;

    Context(const std::vector<std::string>& cmd);

    void resolvePackageContext(const std::string& package);

    int run();

    const decltype(input)& getInput() { return input; }
};

}
