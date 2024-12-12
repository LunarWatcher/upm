#pragma once

#include <stc/FS.hpp>
#include <string>
#include <utility>
#include <vector>

namespace upm {

/**
 * This contains the stdlib of activators, attempting to fit as many of the known patterns as possible.
 * These are exposed via the Lua interface, as these can be defined via the Lua API.
 */
namespace Activators {
namespace Utils {

extern std::vector<std::pair<fs::path, fs::path>> recursiveLink(
    const fs::path &source,
    const fs::path &dest,
    const std::string& fileName,
    const std::vector<std::string>& safeDirNames,
    const std::vector<std::string>& ignoredFiles = {}
);

}

/**
 * Universal activator for everything installing into the standard UNIX format; i.e. for packages where
 * <UPM_HOME>/packages/<package>/ contains bin, share, lib, and include.
 */
extern bool recursiveUniversalUNIXLink(std::vector<std::string>& safeDirNames);

}

}
