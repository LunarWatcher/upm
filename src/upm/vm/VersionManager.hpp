#pragma once

#include "spdlog/spdlog.h"
#include "upm/Context.hpp"
#include "upm/package/Package.hpp"
#include "upm/util/StrUtil.hpp"
#include "upm/util/PathUtils.hpp"

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

inline void disable(Context& ctx) {
    const auto& name = ctx.package;
    // we don't need the version for disabling.
    // Only one version of any given package can be enabled at an arbitrary time anyway.
    if (!ctx.cfg.data.contains("package")) {
        spdlog::error("No packages activated");
        return;
    }
    auto it = ctx.cfg.data.at("package").find(name);
    if (it == ctx.cfg.data.at("package").end()) {
        spdlog::error("{} does not appear to be activated", name);
        return;
    }
    for (auto& sourceDest : *it) {
        auto symlinkDest = sourceDest.at("target");
        fs::path castPath = fs::path(symlinkDest.get<std::string>());
        if (isGoodSymlink(castPath)) {
            spdlog::info("Unlinking {}", castPath.string());
            fs::remove(castPath);
        } else {
            spdlog::error("Skipped bad entry: {}", castPath.string());
        }
    }

    ctx.cfg.data.at("package").erase(it);
    
}

}
