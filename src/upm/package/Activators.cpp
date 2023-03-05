#include "Activators.hpp"

#include "upm/Context.hpp"
#include "upm/conf/Constants.hpp"
#include <iostream>
#include <spdlog/spdlog.h>

#include <vector>
#include <string>

namespace upm {

bool Activators::recursiveUniversalUNIXLink() {
    static const std::vector<std::string> directories = {
        "bin", "share", "lib", "include", "etc"
    };


    auto& ctx = *Context::inst;
    auto prefix = fs::path(ctx.getPrefix());
    if (!fs::exists(prefix)) {
        spdlog::error("Folder {} does not exist.", prefix.string());
        return false;
    }

    spdlog::info("Found match in {}", prefix.string());


    std::vector<std::pair<fs::path, fs::path>> links;
    for (auto& dir : directories) {
        fs::path sourcePath = prefix / dir;
        if (!fs::exists(sourcePath)) {
            // Skip non-existent folders. These are just part of the standard, but not all the
            // folders are required
            continue;
        }
        fs::path destPath   = Constants::APPLY_ROOT / dir;
        if (!fs::exists(destPath)) {
            fs::create_directories(destPath);
        }
        for (auto& path : fs::directory_iterator(sourcePath)) {
            // returns the path relative to sourcePath
            // TODO: see if calling .path() is unnecessary
            std::string fn = path.path().lexically_relative(sourcePath);
            auto linksForDir = Utils::recursiveLink(sourcePath, destPath, fn);
            links.insert(links.end(), linksForDir.begin(), linksForDir.end());
        }
    }
    for (auto& [source, target] : links) {
        spdlog::info("Linking {} -> {}", target.string(), source.string());
        // TODO: use package.<package>.files instead, and add
        // a package.<package>.version
        ctx.cfg.data["package"][ctx.package].push_back({{"target", target.string()}, {"source", source.string()}});
        if (fs::exists(target)) {
            // This is semi-temporary 'til we get uninstallation in place
            fs::remove(target);
        }
        fs::create_directories(target.parent_path());
        fs::create_symlink(source, target);
    }
    ctx.cfg.save();

    return true;
}

// Util defs {{{
std::vector<std::pair<fs::path, fs::path>> Activators::Utils::recursiveLink(const fs::path &source, const fs::path &dest, const std::string& fileName) {
    
    if (fs::is_directory(source / fileName) && fileName != Context::inst->package) {
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
            if (str.find("/opt/upm") == std::string::npos) {
                spdlog::critical("Found existing symlink at {}, but that points to a non-upm directory ({}).", 
                    (dest / fileName).string(), str);
                throw std::runtime_error("Symlink exists, doesn't point to upm");
            }
        }
        return {{source / fileName, dest / fileName}};
    }
}
// }}}

}
