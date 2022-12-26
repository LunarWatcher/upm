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

    /**
     * Utility function for dealing with the install process. This does not deal with any specific installation systems,
     * because different programs are different and require variations here.
     */
    void install();

    /**
     * Utility function for dealing with the activation process. This does not deal with any specific activation systems,
     * because different programs are different and often require minor changes.
     */
    void activate();

    std::string getPrefix();
    std::vector<fs::path> getLuaLookupDirectory();

    std::string locateFile(const std::string& packageName);

    int run();

    const decltype(input)& getInput() { return input; }
};

}
