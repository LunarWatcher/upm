#pragma once

#include <map>
#include <functional>
#include <vector>

#include "PackageResolver.hpp"

namespace gamma {

enum class PackageProvider {
    GITHUB, OTHER
};

struct Package {
    std::string baseURL;
    PackageProvider provider;

    // Optional
    std::vector<std::string> dependencies;

    // Resolves a URL, required if provider is OTHER.
    std::function<std::string(const std::string& version)> resolver;
};

std::map<std::string, Package> packages = {
    { "node.js", {
        "https://nodejs.org/en/download/current/",
        PackageProvider::OTHER,
        {},
        gamma::PackageResolver::ResolveNode
    }},
};

}
