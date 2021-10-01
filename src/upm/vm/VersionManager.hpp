#pragma once

#include "spdlog/spdlog.h"
#include "upm/Context.hpp"
#include "upm/package/Package.hpp"
#include "upm/util/StrUtil.hpp"
#include <iostream>

#include "stc/FS.hpp"

#include <stdexcept>

namespace upm {

inline void enable(const std::string& package, Context& ctx) {
    auto split = StrUtil::splitString(package, "@", 1);
    auto name = split[0];
    auto version = split.size() == 1 ? throw std::runtime_error("Not implemented") : split[1];

    if (packages.find(name) == packages.end()) {
        spdlog::error("Failed to find {}", name);
        return;
    }

    auto& packageInfo = packages.at(name);
    auto packagePath = fs::path("/opt/upm-bin") / (name + "-" + version);
    //fs::create_directory_symlink(std::path("/opt/upm-bin" / (name + "-" + version)));
    packageInfo.enable(packagePath);

}

}
