#pragma once

#include <string>
#include <vector>
#include <cpr/cpr.h>

#include "stc/FS.hpp"

namespace upm {
namespace PackageResolver {

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
    // saved as an option for compatibility purposes
    int stripComponents = -1;
} PackageInfo;

PackageInfo ResolveNode(const std::string& version);

bool EnableBinary(const fs::path& root);

}
}
