#pragma once

#include <string>
#include <vector>
#include <cpr/cpr.h>

namespace gamma {
namespace PackageResolver {

enum class PackageType {
    // Used for installers, such as .debs on Debian and derivatives
    INSTALLER,
    // Packages containing binaries
    BINARY,
    // Packages containing source code. Must be compiled.
    SOURCE
};

typedef struct {
    std::string url;
    PackageType type;
} PackageInfo;

PackageInfo ResolveNode(const std::string& version);

}
}
