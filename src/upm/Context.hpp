#pragma once

#include "upm/api/LuaHelper.hpp"
#include "upm/conf/Config.hpp"
#include "upm/util/Version.hpp"
#include "upm/platform/Platform.hpp"
#include <vector>
#include <string>


namespace upm {

enum class VersionType {
    AT = 0,
    APPROX = 1,
    IMPLICIT = 2
};

typedef struct {
    /**
     * linux, mac
     */
    std::string os;
    /**
     * full CPU identifier; x86, armv<revision>, ...
     */
    std::string cpuArch;
} SysInfo;

class Context {
private:
    std::vector<std::string> input;
public:
    static inline Context* inst = nullptr;

    const SysInfo sysInfo {
#if defined LINUX && defined X86_64
        "linux", "x64"
#elif defined LINUX && defined ARM7
        "linux", "armv7l"
#elif defined LINUX && defined ARM64
        "linux", "arm64"
#elif defined MACOS && defined ARM64
        "darwin", "arm64"
#elif defined MACOS && defined X86_64
        "darwin", "x64"
#elif defined MACOS && defined __arm__
        "darwin", "arm64"
#else
        "UNK", "UNK"
#warning Unknown operating system and CPU configuration. Binary scripts may be unable to function properly.
#endif
    };

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
    void runFile(const std::string& targetFun);

    /**
     * Utility function for dealing with the install process. This does not deal with any specific installation systems,
     * because different programs are different and require variations here.
     */
    void install();

    /**
     * Utility function for dealing with the activation process. This does not deal with any specific activation systems,
     * because different programs are different and often require minor changes.
     */
    void apply();

    void disable();

    std::string getPrefix();
    std::vector<fs::path> getLuaLookupDirectory();

    std::string locateFile(const std::string& packageName);

    int run();

    const decltype(input)& getInput() { return input; }
};

}
