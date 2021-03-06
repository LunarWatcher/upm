#include "upm/Context.hpp"
#include "upm/platform/Platform.hpp"
#include "PackageResolver.hpp"
#include "upm/util/Version.hpp"

#include <filesystem>
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
        "-linux-armv7l.tar.xz", PackageType::BINARY_TAR
#elif defined LINUX && defined ARM64
        "-linux-arm64.tar.xz", PackageType::BINARY_TAR
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

PackageResolver::PackageInfo PackageResolver::ResolvePython(const std::string &version) {
    std::string internalVersion = version;
    if (version == "latest" || version == "lts") {
        cpr::Url url = {
            std::string("https://python.org/ftp/python/")
        };
        cpr::Response r = cpr::Get(
            url
        );
        if (r.status_code != 200) {
            spdlog::error("{}", r.text);
            throw std::runtime_error("python.org/ftp returned bad status code.");
        }

        // As usual, use regex to parse the HTML. Effectively guaranteed to remain constant this time
        const static std::regex pat("<a href=\"((?:\\d.?)+)/\"", std::regex::icase);
        const std::sregex_iterator end;

        std::sregex_iterator it(r.text.begin(), r.text.end(), pat);

        Version vHead{"0.0"}, vStable("0.0");

        for (; it != end; ++it) {
            std::smatch m = *it;
            Version vNew(m[1]);

            if (vNew > vHead) {
                vStable = vHead;
                vHead = vNew;
            }
        }

        if (vHead == "0.0") {
            throw std::runtime_error("Failed to extract version from Python.org");
        }

        internalVersion = vHead.getVersion();

        // We'll want to shorten this down at some point,
        // but I imagine that's better done post-Lua
        // (Note to self: get started on lua already!)
        cpr::Url u("https:/python.org/ftp/python/" + internalVersion + "/Python-" + internalVersion + ".tar.xz");
        auto res = cpr::Head(u);
        // I'm sure there are a lot of ways this could be made more elegant, but here we go anyway.
        // The idea is, retrieve the last two versions.
        // If the head doesn't have a binary, assume the second latest
        // version does.
        //
        // Not sure how Python's release cycles works, but this feels
        // about as bullet-proof as it gets.

        if (res.status_code == 404) {
            internalVersion = vStable.getVersion();
        }

        spdlog::info("Resolved @latest to {}", internalVersion);


    } 
    // If we want support for alpha and beta, this needs some changes. If we
    // decide to use python@3.11.0b1, that's relatively
    // easy to support, but auto-detecting
    // the latest version is painfully annoying.
    // Might be better to stick to stable?
    // I highly doubt anyone is going to use this
    // to get beta versions anyway, but we'll see

    // In either case... sources only. Because Python sucks and doesn't export Linux binaries.
    // Thanks for nothing
    // On the bright side, good excuse to finally implement default compiling.
    return {
        std::string("https://python.org/ftp/python/") + internalVersion + "/Python-" + internalVersion + ".tar.xz", 
        PackageType::SOURCE,
        internalVersion, 1
    };
    throw std::runtime_error("Not configured");
}

std::vector<std::pair<fs::path, fs::path>> PackageResolver::recursiveLink(const fs::path &source, const fs::path &dest, const std::string& fileName) {
    // We don't have to worry about symlinks in the source directory
    if (fs::is_directory(source / fileName) && fs::exists(dest / fileName) && !fs::is_symlink(dest/fileName)) {
        std::vector<std::pair<fs::path, fs::path>> result;
        for (auto& path : fs::directory_iterator(source / fileName)) {
            std::string fn = path.path().lexically_relative(source / fileName);
            auto newVec = recursiveLink(source / fileName, dest / fileName, fn);
            result.insert(result.end(), newVec.begin(), newVec.end());
        }
        return result;
    } else {
        if (fs::exists(dest / fileName)) {
            if (!fs::is_symlink(dest / fileName)) {
                spdlog::critical("Found existing file or non-symlinked directory at {}", (dest/fileName).string());
                throw std::runtime_error("Intended target exists and isn't a symlink");
            }
            auto str = fs::read_symlink(dest/fileName).string();
            // TODO: link in context to make this dynamic
            if (str.find("/opt/upm-bin") == std::string::npos) {
                spdlog::critical("Found existing symlink at {}, but that points to a non-upm directory ({}).", 
                    (dest / fileName).string(), str);
                throw std::runtime_error("Symlink exists, doesn't point to upm");
            }
        }
        return {{source / fileName, dest / fileName}};
    }
}

bool PackageResolver::EnableBinary(const fs::path& root, Context& ctx) {
    static const std::vector<std::string> directories = {
        "bin", "share", "lib", "include"
    };

    spdlog::info("Enabling...");

    std::vector<std::pair<fs::path, fs::path>> links;
    for (auto& dir : directories) {
        fs::path sourcePath = root / dir;
        fs::path destPath   = fs::path("/opt/upm-active") / dir;
        if (!fs::exists(destPath)) {
            fs::create_directories(destPath);
        }
        for (auto& path : fs::directory_iterator(sourcePath)) {
            // returns the path relative to sourcePath
            // TODO: see if calling .path() is unnecessary
            std::string fn = path.path().lexically_relative(sourcePath);
            auto linksForDir = recursiveLink(sourcePath, destPath, fn);
            links.insert(links.end(), linksForDir.begin(), linksForDir.end());
        }
    }
    for (auto& [source, target] : links) {
        spdlog::info("Linking {} -> {}", target.string(), source.string());
        ctx.cfg.data["package"][ctx.package].push_back({{"target", target.string()}, {"source", source.string()}});
        if (fs::exists(target)) {
            // This is semi-temporary 'til we get uninstallation in place
            fs::remove(target);
        }

        fs::create_symlink(source, target);
    }

    spdlog::info("Successfully enabled package.");
    return true;
}

}
