#include "VersionResolvers.hpp"

#include <fstream>
#include "upm/Context.hpp"
#include "stc/Environment.hpp"
#include <unistd.h>

namespace upm {

std::string VersionResolvers::git(const std::string &repoPath, bool vPrefix) {
    // Note; no version capability checks are performed here. These are performed
    // via version declarations in the object.
    std::string version = Context::inst->packageVersion;
    int statusCode = 0;
    std::string res;
    auto cd = "cd " + repoPath + " && ";

    switch (Context::inst->versionType) {
    case VersionType::AT:
        if (version == "lts") {
            // Git repos using lts have to figure out their own decision system.
            // There isn't an easy way to identify LTS
            throw std::runtime_error("lts definitions aren't permitted for generic git repos.");
        } else if (version == "nightly") {
            // This ensures a type of pseudo-tag is returned.
            res = stc::syscommand(cd + "git describe --tags --abbrev=1", &statusCode);
            // sadly, the above command only works in repos with tags.
            // If one can't be found, use the hash.
            // This isn't optimal, but it works.
            if (statusCode != 0) {
                res = stc::syscommand(cd + "git rev-parse --short HEAD", &statusCode);
            }
        } else if (version == "latest") {
            // Otherwise, return the most recent tag.
            res = stc::syscommand(cd + "git describe --tags --abbrev=0", &statusCode);
            // Unlike nightly, this doesn't have a fallback to refer to the latest
            // commit, because this tag is specifically for 
        } else {
            if (vPrefix && version[0] != 'v') version = "v" + version;
            res = stc::syscommand(cd + "git tag -l " + version, &statusCode);
            if (res == version || res == version + "\n") {
                res = version;
            } else statusCode = 1;
        }
        break;
    case VersionType::APPROX:
        if (vPrefix && version[0] != 'v') version = "v" + version;
        // the star doesn't have to be escaped in plain sh, but there isn't necessarily a guarantee
        // sh is the runner.
        // This ensures compatibility on the off chance /bin/sh isn't the runner for `popen`
        // This returns the last tag matching the pattern provided.
        //
        // This particular category, by definition, does not have identifiers. They're for cases where
        // "I know I need v<x>, but I'd like the most recent version matching these criteria without googling"
        res = stc::syscommand(cd + "git describe --tags --abbrev=0 --match " + version + "\\*", &statusCode);
        break;
    default:
        // Should never be invoked; this is a catch-all if different versions are added in the future
        throw std::runtime_error("Illegal versionType used; the git version resolver needs an update");
    }
    if (statusCode == 0) {
        auto resolvedVersion = res;

        // Ensure newlines are stripped from the output
        if (auto index = resolvedVersion.find('\n'); index != std::string::npos) {
            resolvedVersion = resolvedVersion.substr(0, index);
        }
        Context::inst->resolvedPackageVersion = resolvedVersion;
        stc::syscommand((cd + "git checkout " + resolvedVersion).c_str(), &statusCode);
        if (statusCode != 0) {
            throw std::runtime_error("Failed to checkout " + resolvedVersion);
        }
        spdlog::info("Resolved package to version {}", resolvedVersion);
        return resolvedVersion;
    } else throw std::runtime_error("Git failed to return an appropriate status code. stdout: " + res);
}

}
