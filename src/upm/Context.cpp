#include "Context.hpp"

#include <filesystem>
#include <iostream>
#include <algorithm>

#include "InstallationResolver.hpp"
#include "stc/Environment.hpp"
#include "upm/conf/Constants.hpp"
#include "vm/VersionManager.hpp"

// not even sure if this matters, I'm sure there's an include somewhere else
// that fails the Windows build, that's built earlier in the translation unit.
// Maybe
// Idk
#ifdef _WIN32
#error "Windows isn't supported because of a lack of a universal binary dir."
#else
#include <unistd.h>
#endif

namespace upm {

Context::Context(const std::vector<std::string>& cmd) : input(cmd), isRoot(!getuid()), cfg(this) {
    Context::inst = this;
    
    // TODO: nasty hack; find a better way to lateinit or init after setting inst
    helper.init();
    //package = "vim";
    //packageVersion = "latest";
    //helper.runFile("vim.lua");
}

void Context::resolvePackageContext(const std::string& rawVersion) {
    int at = 0, approx = 0;
    for (auto& character : rawVersion) {
        if (character == '@') ++at;
        else if (character == '~') ++approx;
    }

    if (at == 0 && approx == 0) {
        // No version specified is good; we return latest
        package = rawVersion;
        packageVersion = "latest";
        versionType = VersionType::IMPLICIT;
    } else if (at != 1 && approx != 1) {
        spdlog::error("{} doesn't follow any of the supported version formats", rawVersion);
        // Both @ and ~, and we throw.
        throw std::runtime_error("Invalid version format");
    }
    // We split by whichever thing exists

    if (at != 0) {
        auto split = StrUtil::splitString(rawVersion, "@", 1);
        if (split.size() != 2 || split[1].size() == 0) {
            spdlog::error("Invalid format: {} (expected @<version> or ~<version>)", rawVersion);
            throw std::runtime_error("Failed to extract version.");
        }
        package = split[0];
        packageVersion = split[1];
        versionType = VersionType::AT;
    } else {
        auto split = StrUtil::splitString(rawVersion, "~", 1);
        if (split.size() != 2 || split[1].size() == 0) {
            spdlog::error("Invalid format: {} (expected @<version> or ~<version>)", rawVersion);
            throw std::runtime_error("Failed to extract version.");
        }
        package = split[0];
        packageVersion = split[1];
        versionType = VersionType::APPROX;
    }
}

int Context::run() {
    std::string command = input[0];
    input.erase(input.begin());
    
    if (command == "help") {
        std::cout << R"(Commands
    help                Shows this helpful message
    install             Installs one or more packages
    uninstall           Uninstalls one or more packages
    upgrade             Upgrades a package.
    apply               Applies a specific version of a package
    deactivate          Deactivates a package
    list                Lists installed packages, along with installed versions.

Since upm allows multiple installed versions of certain programs,
install and uninstall may require a version to work.

# Upgrading versioned packages
Dependent on issues #1, #4, #6 (read: TBD)

# Updating upm

# License

upm and upm's source code is distributed under the MIT license.
See GitHub for the full license.

    https://github.com/LunarWatcher/upm/blob/master/LICENSE
)" << std::endl;
    } else if (command == "install") {
        if (input.size() < 1) {
            spdlog::error("What package?");
            return -1;
        }
        if (!isRoot) {
            spdlog::error("Please run upm as sudo to install this package.\n"
                          "If you meant to install it for your user, remember to pass --local.");
            return -1;
        }
        resolvePackageContext(input[0]);
        resolve(input[0], *this);
    } else if (command == "apply") {
        if (input.size() < 1) {
            spdlog::error("What package?");
            return -1;
        }
        if (!isRoot) {
            spdlog::error("Please run upm as sudo to apply this package.\n"
                          "If you meant to install it for your user, remember to pass --local.");
            return -1;
        }
        resolvePackageContext(input[0]);
        enable(*this);
    } else if (command == "deactivate") {
        if (input.size() < 1) {
            spdlog::error("What package?");
            return -1;
        }
        if (!isRoot) {
            spdlog::error("Please run upm as sudo to install this package.\n"
                          "If you meant to install it for your user, remember to pass --local.");
            return -1;
        }
        package = input[0];
        versionType = VersionType::AT;
        disable(*this);
    } else {
        // Commands part of the help, but that aren't implemented yet are still unknown.
        // (read: they're not bugs, for the record :) )
        spdlog::error("Unknown command: {}", command);
        return -1;
    }

    return 0;
}

void Context::install() {
    
}

void Context::activate() {

}

std::string Context::getPrefix() {
    fs::path root;
    if (isRoot) {
        root = "/opt";
    } else root = stc::getHome() / ".local";

    root /= "upm/packages";

    // This is a temporary hack; this needs to represent the real resolved version
    // TODO: replace with an argument retrieved from the API
    root /= package + "-" + packageVersion;
    return root.string();
}

std::vector<fs::path> Context::getLuaLookupDirectory() {
    // Getting lookup directories is a complicated process.
    //
    // The scripts are packaged in two different ways; user-specific stuff, and global stuff.
    // Global stuff is always loaded, but user stuff is only loaded when not running as root, primarily
    // to avoid privilege escalation via scripts.
    // Therefore, the first check is to grab all the script paths from the global directory:
    std::vector<fs::path> res;
#ifdef UPM_DEBUG
    // If built as debug, make sure ./lua is included first in the search path.
    res.push_back("./lua/upm");
#endif

    // Each subfolder of the lua folder represents one repository, though I'm not sure how I want to set that up yet.
    // It's primarily set up this way to open for less refactoring.
    res.push_back("/opt/upm/lua/upm/");


    if (!isRoot) {
        // TODO: set up per-user stuff
    }

    return res;
}

}
