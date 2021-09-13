#pragma once

#include <map>
#include <functional>
#include <vector>

#include "PackageResolver.hpp"

#include "stc/FS.hpp"

namespace upm {

enum class PackageProvider {
    GITHUB,
    OTHER,
    /**
     * Used for when the baseURL is actually a key for another package.
     * Because I can't be arsed, aliases CANNOT be nested. An alias MUST
     * point to a proper package.
     */
    ALIAS,
};

struct Package {
    // Base URL, required if the provider isn't OTHER.
    std::string baseURL;
    PackageProvider provider;

    std::function<bool(const fs::path& packageDir)> enable;

    // Optional
    std::vector<std::string> dependencies;

    // Resolves a URL, required if provider is OTHER.
    std::function<PackageResolver::PackageInfo(const std::string& version)> resolver;
};

inline std::map<std::string, Package> packages = {
    { "node.js", {
        "",
        PackageProvider::OTHER,
        upm::PackageResolver::EnableNode,
        {},
        upm::PackageResolver::ResolveNode
    }},
    // python == python3.
    // There's no reason what so ever to give people an easy way to install python 2.
    // We're no longer in the dark ages - it's dead. Upgrade already.
    { "python",{
        "",
        PackageProvider::OTHER,
        nullptr,
        {},
        nullptr // TODO
    }},
    {"python3",{
        "python",
        PackageProvider::ALIAS
    }},
};

}
