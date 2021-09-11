#pragma once

#include <string>
#include <iostream>

#include "upm/util/StrUtil.hpp"
#include "package/Package.hpp"

namespace upm {


inline void resolve(const std::string& package) {
    auto split = StrUtil::splitString(package, "@", 1);
    auto name = split[0];
    auto version = split.size() == 1 ? "latest" : split[1];

    if (packages.find(name) == packages.end()) {
        std::cout << "Failed to find " << name << std::endl;
    }
    auto& packageInfo = packages.at(name);
    switch (packageInfo.provider) {
    case PackageProvider::ALIAS:
        resolve(packageInfo.baseURL);
        break;
    case PackageProvider::OTHER: {
        auto resolvedURL = packageInfo.resolver(version);
        } break;
    case PackageProvider::GITHUB: {

        } break;
    }
}

}
