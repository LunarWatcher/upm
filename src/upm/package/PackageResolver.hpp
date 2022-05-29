#pragma once

#include <string>
#include <vector>
#include <cpr/cpr.h>
#include "upm/Context.hpp"

#include "stc/FS.hpp"

namespace upm {
namespace PackageResolver {

// Not really sure if this makes sense as a strategy anymore.
// Kinda feels more obvious for the invoking function to deal with this on its own;
// that way, we can also supply library methods through Lua when that time
// comes.
// I.e. a function for configure and make, one for cmake, one for debs/equivalent, ...
// Will have to refactor later.
enum class PackageType {
    // Used for installers, such as .debs on Debian and derivatives
    INSTALLER,
    // Packages containing binaries
    BINARY,
    // Packages containing source code. Must be compiled.
    SOURCE,
    // Tars that're meant to be unpacked directly into a system folder.
    // These are a separate category purely because it's easier to keep
    // track of than raw binaries.
    BINARY_TAR,
};

typedef struct _PackageInfo {
    std::string url;
    PackageType type;
    std::string resolvedVersion;
    // Only necessary for tars;
    // saved as an option for format compatibility purposes
    int stripComponents = -1;
} PackageInfo;

std::vector<std::pair<fs::path, fs::path>> recursiveLink(const fs::path& source, const fs::path& dest, const std::string& fileName);

PackageInfo ResolveNode(const std::string& version);
PackageInfo ResolvePython(const std::string& version);

bool EnableBinary(const fs::path& root, Context& ctx);

}
}
