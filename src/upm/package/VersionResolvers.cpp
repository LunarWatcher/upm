#include "VersionResolvers.hpp"

#include <fstream>
#include "upm/Context.hpp"
#include "stc/Environment.hpp"

namespace upm {

std::string VersionResolvers::git(const std::string &repoPath) {
    // Note; no version capability checks are performed here. These are performed
    // via version declarations in the object.
    auto version = Context::inst->packageVersion;

    switch (Context::inst->versionType) {
    case VersionType::AT:
        if (version == "lts") {
            throw std::runtime_error("lts definitions aren't permitted for generic git repos.");
        } else if (version == "nightly") {
            // This ensures a type of pseudo-tag is returned.
            return stc::syscommand("git describe --tags");
        } else if (version == "latest") {
            return stc::syscommand("git describe --tags --abbrev=0");
        }
        break;
    case VersionType::APPROX:
        return stc::syscommand("git describe --tags --abbrev=0 --match " + version  + "*")
        break;
    
    }

}

}
