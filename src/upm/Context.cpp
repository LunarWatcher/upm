#include "Context.hpp"

#include <filesystem>
#include <iostream>

#include "stc/StringUtil.hpp"

#include "upm/util/PathUtils.hpp"
#include "upm/conf/Constants.hpp"

#ifdef _WIN32
#error "Windows isn't supported because of a lack of a universal binary dir."
#else
#include <unistd.h>
#endif

namespace upm {

Context::Context(const std::vector<std::string>& cmd) : input(cmd), isRoot(getuid() == 0), cfg(this) {
    Context::inst = this;
    if (!isRoot) {
        throw std::runtime_error("upm must be run as root");
    }

    helper.init();
}

void Context::resolvePackageContext(const std::string& rawVersion) {
    int at = 0;
    int approx = 0;
    for (const auto& character : rawVersion) {
        if (character == '@') {
            ++at;
        } else if (character == '~') {
            ++approx;
        }
    }

    if (at == 0 && approx == 0) {
        // No version specified is good; we return latest
        package = rawVersion;
        packageVersion = "latest";
        versionType = VersionType::AT;
    } else if (at != 1 && approx != 1) {
        spdlog::error("{} doesn't follow any of the supported version formats", rawVersion);
        // Both @ and ~, and we throw.
        throw std::runtime_error("Invalid version format");
    } else {
        // We split by whichever thing exists

        if (at != 0) {
            auto split = stc::string::split(rawVersion, "@", 1);
            if (split.size() != 2 || split[1].empty()) {
                spdlog::error("Invalid format: {} (expected @<version> or ~<version>)", rawVersion);
                throw std::runtime_error("Failed to extract version.");
            }
            package = split[0];
            packageVersion = split[1];
            versionType = VersionType::AT;
        } else {
            auto split = stc::string::split(rawVersion, "~", 1);
            if (split.size() != 2 || split[1].empty()) {
                spdlog::error("Invalid format: {} (expected @<version> or ~<version>)", rawVersion);
                throw std::runtime_error("Failed to extract version.");
            }
            package = split[0];
            packageVersion = split[1];
            versionType = VersionType::APPROX;
        }
    }

    loadPackage();

}

void Context::parseFlags() {
    static std::map<char, std::string> shorthands = {
        {'f', "force"}
    };
    static std::map<std::string, bool> argc = {
        {"force", false}
    };
    enum State {
        STATE_NEW,
        STATE_VALUE
    };
    std::string k;
    
    State state = STATE_NEW;
    for (auto& line : input) {
        switch (state) {
        case STATE_NEW: {
            if (line.size() < 2) {
                continue;
            }
            if (line.at(0) == '-') {
                if (line.at(1) == '-') {
                    if (line.size() == 2) {
                        // too short to be valid
                        continue;
                    }
                    auto parts = stc::string::split(line, '=', 1);
                    k = parts[0].substr(2);

                    if (parts.size() == 1) {
                        if (argc[k]) {
                            state = STATE_VALUE;
                        } else {
                            flags[k] = "true";
                        }
                    } else {
                        flags[k] = parts[1];
                    }
                } else {
                    // -f
                    for (size_t i = 1; i < line.size(); ++i) {
                        if (shorthands.find(line[i]) != shorthands.end()) {
                            flags[shorthands.at(line[i])] = "true";
                        }
                    }

                }
            }
        } break;
        case STATE_VALUE: {
            flags[k] = line;
            state = STATE_NEW;
        } break;
        }
    }

}

int Context::run() {
    std::string command = input[0];
    input.erase(input.begin());
    parseFlags();
    
    if (command == "help") {
        std::cout << R"(Commands
    help					Shows this helpful message
    install					Installs one or more packages
    uninstall, remove		Uninstalls one or more packages
    upgrade					Upgrades a package.
    apply					Applies a specific version of a package
    deactivate, disable		Deactivates a package
    list					Lists installed packages, along with installed versions.
    version					Shows information about the currently installed version of upm

Since upm allows multiple installed versions of certain programs,
install and uninstall may require a version to work.

# License

upm and upm's source code is distributed under the MIT license.
See GitHub for the full license.

    https://github.com/LunarWatcher/upm/blob/master/LICENSE
)" << std::endl;
    } else if (command == "install") {
        if (input.empty()) {
            spdlog::error("What package?");
            return -1;
        }

        if (!std::filesystem::is_directory(Constants::UPM_ROOT)) {
            std::filesystem::create_directories(Constants::UPM_ROOT);
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
        if (input.empty()) {
            spdlog::error("What package?");
            return -1;
        }
        if (cfg.data.contains("package") && cfg.data.at("package").contains(package)) {
            spdlog::error(package + " is already active. Deactivate it before trying again");
            return -1;
        }
        resolvePackageContext(input[0]);
        spdlog::info("Resolved version to {}", resolvedPackageVersion.empty() ? packageVersion : resolvedPackageVersion);
        apply();
        spdlog::info("Successfully activated " + package);
    } else if (command == "deactivate" || command == "disable") {
        if (input.empty()) {
            spdlog::error("What package?");
            return -1;
        }
        resolvePackageContext(input[0]);
        versionType = VersionType::AT;
        disable();
    } else if (command == "uninstall" || command == "remove") {
        spdlog::info("Will fix later :)");
    } else if (command == "version") {
        std::cout << "upm version " << 
#ifdef UPM_VERSION
            UPM_VERSION
#else
            "unknown"
#endif
            << std::endl;
    } else {
        // Commands part of the help, but that aren't implemented yet are still unknown.
        // (read: they're not bugs, for the record :) )
        spdlog::error("Unknown command: {}", command);
        return -1;
    }

    return 0;
}

void Context::install() {
    helper.invoke(package, "install");
}

void Context::apply() {
    resolveSemanticMarkers();
    helper.invoke(package, "apply");
}

void Context::configureSemanticMarkers() {
    if (resolvedPackageVersion.empty()) {
        // No version resolution, no marker resolution for you
        return;
    }

    if (versionType == VersionType::AT && (packageVersion == "latest" || packageVersion == "lts" || packageVersion == "nightly")) {
        // No extended checks are needed here. If we install a version using a semantic marker, that semantic marker is now that version.
        cfg["semanticMarkers"][package][packageVersion] = resolvedPackageVersion;
        cfg.save();
    }
    // We _could_ check for exact versions and update the existing definitions, but why? The majority of the time people specify versions,
    // it's legacy versions or a version that's ahead of the current version, but still oddly specifically required for some reason:tm:.
    // This does mean if someone @latest grabbing 1.2.2, and then later @1.2.3, the latest head won't be moved. This also helps upgrade
    // semantics, as this lets us track uniquely installed versions vs. semantic installs following some pre-defined "version pointer"
    // of sorts.
    //
    // I mean, that's effectively what latest, lts, and nightly all are.
}

void Context::resolveSemanticMarkers() {
    // See the comments in configureSemanticMarkers (that are right above this function at the time of writing) for some context
    //
    // Since semantic markers are only meant to follow the version pointers from some upstream source, there's v ery little to do here.
    // Now, while an equivalent check to what configureSemanticMarkers does, there isn't a need to. We just check for the existence
    // of the supplied package version in the semantic markers map for the package.
    //
    // If it isn't there, we don't know much about its validity as a semantic marker.
    // If it is there, we know it was defined by configureSemanticMarkers (barring fuckery from the user, but that's their fault),
    // and that it therefore is a valid semantic marker.
    //
    // It saves redundancy in the event future markers are added, as they only need to be updated there (and potentially in some
    // consumer scripts), and nowhere else.
    if (versionType == VersionType::AT
        && cfg.data.contains("semanticMarkers")
        && cfg.data.at("semanticMarkers").contains(package)
        && cfg.data.at("semanticMarkers").at(package).contains(packageVersion)) {
        resolvedPackageVersion = cfg.data.at("semanticMarkers").at(package).at(packageVersion).get<std::string>();
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
        std::filesystem::path castPath = std::filesystem::path(symlinkDest.get<std::string>());
        if (isGoodSymlink(castPath)) {
            spdlog::info("Unlinking {}", castPath.string());
            std::filesystem::remove(castPath);
        } else {
            spdlog::error("Skipped bad entry: {}", castPath.string());
        }
    }

    cfg.data.at("package").erase(it);
    cfg.save();
    spdlog::info("Successfully disabled " + package);
}

void Context::loadPackage() {
    auto fn = locateFile(this->package);
    helper.loadPackage(this->package, fn);

}

std::string Context::locateFile(const std::string& packageName) {
    auto dirs = getLuaLookupDirectory();
    for (auto& dir : dirs) {
        // TODO: this is where alias cache lookup goes
        auto path = dir / (packageName + ".lua");
        if (std::filesystem::is_regular_file(path)) {
            return path;
        }
    }
    return "";
}

bool Context::checkInstalled() {
    auto prefix = getPrefix();
    bool isInstalled = std::filesystem::is_directory(prefix);
    // TODO: use isInstalled + config to check whether or not the existing directory should be removed
    return isInstalled;
}

std::string Context::getPrefix() {
    std::filesystem::path root = Constants::UPM_ROOT / "packages";

    root /= package + "-" + (
        resolvedPackageVersion.empty() ? packageVersion : resolvedPackageVersion
    );
    return root.string();
}

std::vector<std::filesystem::path> Context::getLuaLookupDirectory() {
    std::vector<std::filesystem::path> res;
#ifdef UPM_DEBUG
    // If built as debug, make sure ./lua is included first in the search path.
    res.push_back("./lua/upm");
#else
    if (flags.find("install_self") != flags.end() && package == "upm") {
        spdlog::debug("Installing self; injecting cwd as a lookup path");
        res.push_back("./lua/upm");
    }
#endif

    // Each subfolder of the lua folder represents one repository, though I'm not sure how I want to set that up yet.
    // It's primarily set up this way to open for less refactoring.
    res.push_back(Constants::UPM_ROOT / "lua/upm/");

    return res;
}

}
