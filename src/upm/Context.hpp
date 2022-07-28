#pragma once

#include "upm/api/LuaHelper.hpp"
#include "upm/conf/Config.hpp"
#include "upm/util/Version.hpp"
#include <vector>
#include <string>


namespace upm {

enum class VersionType {
    AT = 0,
    APPROX = 1,
    IMPLICIT = 2
};

class Context {
private:
    std::vector<std::string> input;
public:
    static inline Context* inst = nullptr;

    std::string package;
    std::string packageVersion;
    // TODO: determine the usefulness fo this field in practice, particularly
    // for the Context Lua API (AKA proxying objects is hard)
    VersionType versionType;

    LuaHelper helper;

    const bool isRoot;
    Config cfg;

    Context(const std::vector<std::string>& cmd);

    void resolvePackageContext(const std::string& package);

    int run();

    const decltype(input)& getInput() { return input; }
};

}
