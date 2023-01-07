#include "Context.hpp"

#include <filesystem>
#include <iostream>
#include <algorithm>

#include "stc/Environment.hpp"
#include "stc/StringUtil.hpp"

#include "upm/util/PathUtils.hpp"
#include "upm/conf/Constants.hpp"

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
    if (isRoot) {
        throw std::runtime_error("upm cannot be run as root");
    }
    
    helper.init();
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
        versionType = VersionType::AT;
        return;
    } else if (at != 1 && approx != 1) {
        spdlog::error("{} doesn't follow any of the supported version formats", rawVersion);
        // Both @ and ~, and we throw.
        throw std::runtime_error("Invalid version format");
    }
    // We split by whichever thing exists

    if (at != 0) {
        auto split = stc::string::split(rawVersion, "@", 1);
        if (split.size() != 2 || split[1].size() == 0) {
            spdlog::error("Invalid format: {} (expected @<version> or ~<version>)", rawVersion);
            throw std::runtime_error("Failed to extract version.");
        }
        package = split[0];
        packageVersion = split[1];
        versionType = VersionType::AT;
    } else {
        auto split = stc::string::split(rawVersion, "~", 1);
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
        resolvePackageContext(input[0]);
        install();
        configureSemanticMarkers();
        spdlog::info("Successfully installed " + package);
        spdlog::info("Preparing automatic activation...");
        if (cfg.data.contains("package") && cfg.data.at("package").contains(package)) {
            disable();
        }
        apply();
        spdlog::info("{} activated", package);
    } else if (command == "apply") {
        if (input.size() < 1) {
            spdlog::error("What package?");
            return -1;
        }
        resolvePackageContext(input[0]);
        if (cfg.data.contains("package") && cfg.data.at("package").contains(package)) {
            spdlog::error(package + " is already active. Deactivate it before trying again");
            return -1;
        }
        apply();
        spdlog::info("Successfully activated " + package);
    } else if (command == "deactivate") {
        if (input.size() < 1) {
            spdlog::error("What package?");
            return -1;
        }
        resolvePackageContext(input[0]);
        versionType = VersionType::AT;
        disable();
    } else if (command == "_path") {
        std::cout << "/opt/upm/active/bin" << std::endl;
        return 0;
    } else {
        // Commands part of the help, but that aren't implemented yet are still unknown.
        // (read: they're not bugs, for the record :) )
        spdlog::error("Unknown command: {}", command);
        return -1;
    }

    return 0;
}

void Context::install() {
    runFile("install");
}

void Context::apply() {
    resolveSemanticMarkers();
    runFile("apply");
}

void Context::configureSemanticMarkers() {

    if (versionType == VersionType::AT && (packageVersion == "latest" || packageVersion == "lts" || packageVersion == "nightly")) {
        if (resolvedPackageVersion == "") {
            // No version resolution, no marker resolution. 
            return;
        }
        // No extended checks are needed here. If we install a version using a semantic marker, that semantic marker is now that version.
        cfg["semanticMarkers"][package][packageVersion] = resolvedPackageVersion;
    } else {
        // TODO
    }
    cfg.save();
}

void Context::resolveSemanticMarkers() {
    if (versionType == VersionType::AT && (packageVersion == "latest" || packageVersion == "lts" || packageVersion == "nightly")) {
        if (cfg.data.contains("semanticMarkers")
            && cfg.data.at("semanticMarkers").contains(package)
            && cfg.data.at("semanticMarkers").at(package).contains(packageVersion)) {
            resolvedPackageVersion = cfg.data.at("semanticMarkers").at(package).at(packageVersion).get<std::string>();
        }
    }
}

void Context::disable() {
    // we don't need the version for disabling.
    // Only one version of any given package can be enabled at an arbitrary time anyway.
    if (!cfg.data.contains("package")) {
        spdlog::error("No packages activated");
        return;
    }
    auto it = cfg.data.at("package").find(package);
    if (it == cfg.data.at("package").end()) {
        spdlog::error("{} does not appear to be activated", package);
        return;
    }
    for (auto& sourceDest : *it) {
        auto symlinkDest = sourceDest.at("target");
        fs::path castPath = fs::path(symlinkDest.get<std::string>());
        if (isGoodSymlink(castPath)) {
            spdlog::info("Unlinking {}", castPath.string());
            fs::remove(castPath);
        } else {
            spdlog::error("Skipped bad entry: {}", castPath.string());
        }
    }

    cfg.data.at("package").erase(it);
    cfg.save();
    spdlog::info("Successfully disabled " + package);
}

void Context::runFile(const std::string& targetFun) {
    auto res = locateFile(this->package);
    if (res == "") {
        throw std::runtime_error("Failed to find a Lua file for " + this->package);
    }

    helper.runFileForResult(res, 1, {LUA_TTABLE});
    lua_getfield(helper.getState(), -1, targetFun.c_str());
    if (lua_isnil(helper.getState(), -1)) {
        throw std::runtime_error("Must define a function for " + targetFun);
    }
    if (lua_pcall(helper.getState(), 0, 0, 0) != LUA_OK) {
        if (lua_isstring(*helper, -1)) {
            spdlog::error(lua_tostring(*helper, -1));
        } else {
            helper.dump();
        }
        throw std::runtime_error("A critical Lua failure occurred.");
    }
    lua_pop(helper.getState(), lua_gettop(helper.getState()));
}

std::string Context::locateFile(const std::string& packageName) {
    auto dirs = getLuaLookupDirectory();
    for (auto& dir : dirs) {
        // TODO: this is where alias cache lookup goes
        auto path = dir / (packageName + ".lua");
        if (fs::is_regular_file(path)) {
            return path;
        }
    }
    return "";
}

bool Context::checkInstalled() {
    auto prefix = getPrefix();
    bool isInstalled = fs::is_directory(prefix);
    // TODO: use isInstalled + config to check whether or not the existing directory should be removed
    return isInstalled;
}

std::string Context::getPrefix() {
    fs::path root = "/opt/upm/packages";

    root /= package + "-" + (
        resolvedPackageVersion == "" ? packageVersion : resolvedPackageVersion
    );
    return root.string();
}

std::vector<fs::path> Context::getLuaLookupDirectory() {
    std::vector<fs::path> res;
#ifdef UPM_DEBUG
    // If built as debug, make sure ./lua is included first in the search path.
    res.push_back("./lua/upm");
#endif

    // Each subfolder of the lua folder represents one repository, though I'm not sure how I want to set that up yet.
    // It's primarily set up this way to open for less refactoring.
    res.push_back("/opt/upm/lua/upm/");


    //if (!isRoot) {
        //// TODO: set up per-user stuff
    //}

    return res;
}

}
