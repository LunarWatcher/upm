#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <regex>


#include "spdlog/spdlog.h"
#include "stc/FS.hpp"
#include "stc/Environment.hpp"
#include "upm/Context.hpp"
#include "upm/util/StrUtil.hpp"
#include "package/Package.hpp"
#include "cpr/cpr.h"

namespace upm {

inline bool unpackTar(const fs::path& source, const fs::path& dest, int stripComponents) {
    std::string ext = source.filename();
    std::string tarArg = "";
    // Tar on Ubuntu is allegedly supposed to be able to figure out the type on its own.
    // Something about GNU tar or whatever
    // For compatibility, we're rolling with explicit flags.
    // May be removed if I find any better information on the topic
    //if (std::regex_search(ext, std::regex{"\\.tar\\.gz$"})) {
        //tarArg = "--gzip";
    //} else if (std::regex_search(ext, std::regex{"\\.tar\\.xz$"})) {
        //tarArg = "--xz";
        //std::cout << ".xz detected" << std::endl;
    //}
    //if (tarArg == "") {
        //std::cerr << "Failed to resolve tar argument for " << ext << std::endl;
        //return false;
    //}
    if (stripComponents > 0) {
        tarArg += "--strip-components " + std::to_string(stripComponents);
    }
    tarArg += " -xf " + source.string();
    tarArg += " -C " + dest.string();
    spdlog::info("Unpacking tar to {}", dest.string());
    fs::create_directories(dest);
    // ... why not just check the exit code?
    auto res = stc::syscommand("tar " + tarArg);
    if (res != "") {
        spdlog::error("Tar failed: {}", res);
    }
    return res == "";

}

inline fs::path download(const std::string& url) {
    spdlog::info("Downloading {}...", url);
    auto name = url.substr(url.rfind('/') + 1);
    fs::path tmpDir = fs::temp_directory_path();
    fs::path p = tmpDir / ("upm-" + name);
    if (fs::exists(p)) {
        spdlog::info("Cache entry found at {}.", p.string());
        return p.string();
    }

    std::ofstream of(
        p.string()
    );

    auto r = cpr::Download(
        of,
        cpr::Url{url}
    );
    spdlog::debug("Received {}Mb", ((double) r.downloaded_bytes) / (1024.0 * 1024.0));
    if (r.status_code != 200) {
        throw std::runtime_error(std::string{"Download failed with status code"} + std::to_string(r.status_code));
    }
    return p;
}

inline void install(const std::string& url, const std::string& label, int stripComponents, const PackageResolver::PackageType& packageType) {
    // make sure the cache exists
    if (!fs::exists(fs::path{"/opt"}/ "upm-bin")) {
        fs::create_directory(fs::path{"/opt"} / "upm-bin");
    } else if (fs::exists(fs::path{"/opt/upm-bin/"} / label)) {
        spdlog::info("{} is already installed", label);
        return;
    }
    // TODO: check local vs root before determining the install location
    auto file = download(url);

    auto dest = fs::path{"/opt"} / "upm-bin" / label;
    if (packageType == PackageResolver::PackageType::BINARY_TAR) {
        auto unpacked = unpackTar(file, dest, stripComponents);
        if (unpacked) {
            spdlog::info("Successfully installed {}", label);
        } else {
            spdlog::error("An error occured when unpacking the tar");
        }
        return;
    } else if (packageType == PackageResolver::PackageType::SOURCE) {
        auto srcDest = dest;
        srcDest.remove_filename();
        auto fCopy = file;
        // nasty hack
        srcDest /= fCopy.replace_extension()
            .replace_extension();

        auto unpacked = unpackTar(file, srcDest,
            stripComponents);
        if (unpacked) {
            spdlog::info("Unpacked source; preparing to compile");

            // Massively incompatible
            int res = std::system(("cd " + srcDest.string()
                + "&& ./configure --prefix=" + dest.string()).c_str());
            if (res != 0) {
                spdlog::error("An error occurred when trying to configure");
                return;
            }
            spdlog::info("Configure done. Compiling...");
            res = std::system(("cd " + srcDest.string()
                // TODO: Sneak in actual thread count here
                + "&& make -j 4 && make install").c_str());
            if (res != 0) {
                spdlog::error("Failed to compile");
                return;
            }
            spdlog::info("Successfully installed {}", label);
        } else {
            spdlog::error("An error occured when unpacking the tar");
        }
    }
}


inline void resolve(const std::string& package, Context& ctx) {
    auto split = StrUtil::splitString(package, "@", 1);
    auto name = split[0];
    auto version = split.size() == 1 ? "latest" : split[1];
    spdlog::info("Installing {}@{}...", name, version);
    ctx.package = name + "@" + version;

    if (packages.find(name) == packages.end()) {
        spdlog::error("Failed to find package: {}", name);
        return;
    }
    auto& packageInfo = packages.at(name);
    switch (packageInfo.provider) {
    case PackageProvider::ALIAS:
        resolve(packageInfo.baseURL, ctx);
        break;
    case PackageProvider::OTHER: {
        auto pInfo = packageInfo.resolver(version);
        install(pInfo.url, name + "-" + pInfo.resolvedVersion, pInfo.stripComponents, pInfo.type);
        } break;
    case PackageProvider::GITHUB: {

        } break;
    }
}

}
