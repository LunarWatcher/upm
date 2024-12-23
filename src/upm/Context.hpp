#pragma once

#include "upm/api/LuaHelper.hpp"
#include "upm/conf/Config.hpp"
#include "upm/platform/Platform.hpp"
#include <vector>
#include <string>


namespace upm {

enum class VersionType {
    AT = 0,
    APPROX = 1,
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

    void parseFlags();
public:
    std::map<std::string, std::string> flags;

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
#elif defined(_WIN32)
#error "Invalid OS"
#else
        "UNK", "UNK"
#warning Unknown operating system and CPU configuration. Binary scripts may be unable to function properly.
#endif
    };

    /**
     * The package name; received as user input
     */
    std::string package;

    /**
     * The package version; received as user input. May represent a semantic string rather
     * than a concrete version; meaning while this could be 6.9.0, it could also be latest,
     * lts, or nightly. Whether these are supported is down to version resolution
     */
    std::string packageVersion;

    /**
     * For symlink purposes, this represents the concrete version. If one isn't defined,
     * packageVersion may be used instead.
     *
     * This is primarily used to symlink semantical version (such as latest or nightly) to
     * concrete versions, as defined by the individual script's version resolution systems.
     */
    std::string resolvedPackageVersion;

    std::map<std::string, std::string> variables;

    VersionType versionType;

    LuaHelper helper;

    const bool isRoot;
    Config cfg;

    Context(const std::vector<std::string>& cmd);

    void resolvePackageContext(const std::string& package);

    void loadPackage();

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

    /**
     * Internal helper function that deals with saving semantic marker information to the config. This particular method is
     * primarily used for installation, uninstallation, and upgrades.
     *
     * This function checks if the version provided by the user matches one of a few predefined strings (see the documentation for an up-to-date-ish list),
     * and stores  the newly installed version in it. If a new version is installed using the same semantic marker later, the previously written value is blatantly overridden
     *
     * This method does not remove any installed versions. This must be addressed manually via the upgrade or removal system, depending on context
     */
    void configureSemanticMarkers();

    /**
     * Queries the semantic marker cache for a relevant version to use.
     */
    void resolveSemanticMarkers();

    void disable();

    bool checkInstalled();

    std::string getPrefix();
    std::vector<std::filesystem::path> getLuaLookupDirectory();

    std::string locateFile(const std::string& packageName);

    int run();

    const decltype(input)& getInput() { return input; }
};

}
