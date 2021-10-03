#pragma once

#include "spdlog/spdlog.h"
#include "upm/Context.hpp"
#include "upm/package/Package.hpp"
#include "upm/util/StrUtil.hpp"
#include <iostream>

#include "stc/FS.hpp"

#include <stdexcept>

namespace upm {

inline void enable(Context& ctx) {
    const auto& name = ctx.package;
    const auto& version = ctx.packageVersion;

    if (packages.find(name) == packages.end()) {
        spdlog::error("Failed to find {}", name);
        return;
    }

    auto& packageInfo = packages.at(name);
    auto packagePath = fs::path("/opt/upm-bin") / (name + "-" + version);
    packageInfo.enable(packagePath, ctx);

}

}
