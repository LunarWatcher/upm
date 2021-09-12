#pragma once

#include <string>
#include <iostream>
#include <fstream>


#include "stc/FS.hpp"
#include "upm/util/StrUtil.hpp"
#include "package/Package.hpp"
#include "cpr/cpr.h"

namespace upm {

inline std::string download(const std::string& url) {
    std::cout << "Downloading " << url << "..." << std::endl;
    auto name = url.substr(url.rfind('/') + 1);
    fs::path tmpDir = fs::temp_directory_path();
    fs::path p = tmpDir / ("upm-" + name);
    if (fs::exists(p)) {
        std::cout << "Cache entry found at " << p.string() << std::endl;
        return p.string();
    }

    std::ofstream of(
        p.string()
    );

    auto r = cpr::Download(
        of,
        cpr::Url{url}
    );
    std::cout << "Received " << (((double) r.downloaded_bytes) / (1024.0 * 1024.0)) << "Mb" << std::endl;
    if (r.status_code != 200) {
        throw std::runtime_error(std::string{"Download failed with status code"} + std::to_string(r.status_code));
    }
    return p.string();
}

inline void resolve(const std::string& package) {
    auto split = StrUtil::splitString(package, "@", 1);
    auto name = split[0];
    auto version = split.size() == 1 ? "latest" : split[1];
    std::cout << "Installing " << name << "@" << version << std::endl;

    if (packages.find(name) == packages.end()) {
        std::cout << "Failed to find " << name << std::endl;
        return;
    }
    auto& packageInfo = packages.at(name);
    switch (packageInfo.provider) {
    case PackageProvider::ALIAS:
        resolve(packageInfo.baseURL);
        break;
    case PackageProvider::OTHER: {
        auto resolvedURL = packageInfo.resolver(version);
        download(resolvedURL.url);
        } break;
    case PackageProvider::GITHUB: {

        } break;
    }
}

}
