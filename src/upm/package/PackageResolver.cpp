#include "upm/platform/Platform.hpp"
#include "PackageResolver.hpp"

#include <stdexcept>
#include <regex>
#include <iostream>

#include <spdlog/spdlog.h>

namespace upm {

PackageResolver::PackageInfo PackageResolver::ResolveNode(const std::string& version) {
    std::string internalVersion = version;
    // TODO: handle binary arch
    if (version == "latest" || version == "lts") {
        // we need some extra, and mildly annoying, additional checks for these lot.
        // The concept of "latest" in general requires determining what the newest version is.
        // LTS is worse because LTS is a human-defined concept that machines can't infer.
        //
        // I mean, is /en/download/latest and /en/download/lts really that much to ask for?
        // ... well, yes. It's JavaScript, it fantastic at doing everything awfully.
        cpr::Url url = {
            std::string("https://nodejs.org/en/download/")
                + (version == "latest" ? "current" : "")
        };
        cpr::Response r = cpr::Get(
            url
        );
        if (r.status_code != 200) {
            throw std::runtime_error("nodejs.org returned bad status code.");
        }
        // This part is really simple; we just need to find the version.
        // We leave URL validation to InstallationResolver
        const std::regex pat("Latest (?:Current|LTS) Version: <strong>(\\d+\\.\\d+\\.\\d+)</strong> \\(includes npm", std::regex::icase);
        std::smatch sm;
        if (!regex_search(r.text, sm, pat)) {
            std::cerr << r.text << std::endl;
            throw std::runtime_error("Failed to extract version");
        }

        internalVersion = sm[1];
        std::cout << "Detected version " << internalVersion << std::endl;

    } 
    return {
        std::string("https://nodejs.org/dist/v") + internalVersion + "/node-v" + internalVersion +
#if defined LINUX && defined X86_64
        "-linux-x64.tar.xz", PackageType::BINARY_TAR
#elif defined LINUX && defined ARM7
        "-linux-arm64.tar.xz", PackageType::BINARY_TAR
#elif defined LINUX && defined ARM64
        "-linux-armv7l.tar.xz", PackageType::BINARY_TAR
#elif defined MACOS && defined ARM64
       "-darwin-arm64.tar.gz", PackageType::BINARY_TAR
#elif defined MACOS && defined X86_64
        "-darwin-x64.tar.gz", PackageType::BINARY_TAR
#else
        ".tar.gz", PackageType::SOURCE
#endif
        , internalVersion, 1
    };
}

bool PackageResolver::EnableBinary(const fs::path& root) {
    static const std::vector<std::string> directories = {
        "bin", "share", "lib", "include"
    };

    spdlog::info("Enabling...");

    for (auto& dir : directories) {
        fs::path sourcePath = root / dir;
        fs::path destPath   = fs::path("/usr/local") / dir;
        for (auto& path : fs::directory_iterator(sourcePath)) {
            // returns the path relative to sourcePath
            // TODO: see if calling .path() is unnecessary
            std::string fn = path.path().lexically_relative(sourcePath);
            //std::cout << (sourcePath / fn).string() << " -> " << (destPath / fn).string() << std::endl;
            try {
                if (fs::is_directory(sourcePath / fn)) {
                    spdlog::warn("Fix directory symlinks, Livi");
                    continue;
                }
                fs::create_symlink(sourcePath / fn, destPath / fn);
            } catch (...) {
                spdlog::warn("Failed to create symlink: dest already exists: {} -> {}", (sourcePath / fn).string(), (destPath / fn).string());
            }
        }
    }

    spdlog::info("Successfully enabled package.");
    return true;
}

}
