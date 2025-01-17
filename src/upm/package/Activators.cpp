#include "Activators.hpp"

#include "upm/Context.hpp"
#include "upm/conf/Constants.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>

#include <thread>
#include <vector>
#include <string>

namespace upm {

bool Activators::recursiveUniversalUNIXLink(std::vector<std::string>& safeDirNames) {
    static const std::vector<std::string> directories = {
        "bin", "share", "lib", "include", "etc"
    };
    static const std::vector<std::string> ignoreFiles = { "mimeinfo.cache" };

    auto& ctx = *Context::inst;
    safeDirNames.push_back(ctx.package);

    auto prefix = std::filesystem::path(ctx.getPrefix());
    if (!std::filesystem::exists(prefix)) {
        spdlog::error("Folder {} does not exist.", prefix.string());
        return false;
    }

    spdlog::info("Found match in {}", prefix.string());


    std::vector<std::pair<std::filesystem::path, std::filesystem::path>> links;
    for (const auto& dir : directories) {
        std::filesystem::path sourcePath = prefix / dir;
        if (!std::filesystem::exists(sourcePath)) {
            // Skip non-existent folders. These are just part of the standard, but not all the
            // folders are required
            continue;
        }
        std::filesystem::path destPath = Constants::APPLY_ROOT / dir;
        if (!std::filesystem::exists(destPath)) {
            std::filesystem::create_directories(destPath);
        }
        for (const auto& path : std::filesystem::directory_iterator(sourcePath)) {
            // returns the path relative to sourcePath
            // TODO: see if calling .path() is unnecessary
            std::string fn = path.path().lexically_relative(sourcePath);
            if (std::find(ignoreFiles.begin(), ignoreFiles.end(), fn) != ignoreFiles.end()) { continue; }
            spdlog::debug("filename: {}", fn);
            auto linksForDir = Utils::recursiveLink(sourcePath, destPath, fn, safeDirNames, ignoreFiles);
            links.insert(links.end(), linksForDir.begin(), linksForDir.end());
        }
    }
    spdlog::info("Built symlink tree. Checking for conflicts...");
    bool good = true;
    for (auto& [source, target] : links) {
        if (std::filesystem::exists(target)) {
            auto allowOverwrite = std::filesystem::is_directory(target) && (
                std::filesystem::is_empty(target)
                || std::find(safeDirNames.begin(), safeDirNames.end(), target.filename()) != safeDirNames.end()
            );

            if (!std::filesystem::is_symlink(target)
                && (
                    !allowOverwrite
                    || !std::filesystem::is_directory(target)
                )
            ) {
                spdlog::warn("Found existing file or non-symlinked directory at {}", target.string());
                good = false;
                continue;
            }
            if (!allowOverwrite) {
                auto str = std::filesystem::read_symlink(target).string();
                // TODO: link in context to make this dynamic
                if (str.find(Constants::UPM_ROOT.string()) == std::string::npos) {
                    spdlog::warn("Found existing symlink at {}, but that points to a non-upm directory ({}).", 
                        target.string(), str);
                    good = false;
                    continue;
                }
            }
        }
    }

    if (!good) {
        if (ctx.flags["force"] == "true") {
            spdlog::warn("Found symlink conflicts, but --force is passed. Install continues in 5 seconds. Press Ctrl-C to abort.");
            std::this_thread::sleep_for(std::chrono::seconds(5));
        } else {
            spdlog::error("One or more symlink conflicts exist. Pass `--force` to ignore.");
            throw std::runtime_error("Symlink target(s) already exist.");
        }
    } else {
        spdlog::info("No path conflicts detected.");
    }

    spdlog::info("Performing symlinks.");
    for (auto& [source, target] : links) {
        spdlog::info("Linking {} -> {}", target.string(), source.string());
        // TODO: use package.<package>.files instead, and add
        // a package.<package>.version
        ctx.cfg.data["package"][ctx.package].push_back({{"target", target.string()}, {"source", source.string()}});
        if (std::filesystem::exists(target)) {
            // This is semi-temporary 'til we get uninstallation in place
            std::filesystem::remove_all(target);
        }
        std::filesystem::create_directories(target.parent_path());
        std::filesystem::create_symlink(source, target);
    }
    ctx.cfg.save();

    return true;
}

// Util defs {{{
std::vector<std::pair<std::filesystem::path, std::filesystem::path>> Activators::Utils::recursiveLink(
        const std::filesystem::path &source,
        const std::filesystem::path &dest,
        const std::string& fileName,
        const std::vector<std::string>& safeDirNames,
        const std::vector<std::string>& ignoreFiles
) {
    
    if (std::filesystem::is_directory(source / fileName) && std::find(safeDirNames.begin(), safeDirNames.end(), fileName) == safeDirNames.end()) {
        std::vector<std::pair<std::filesystem::path, std::filesystem::path>> result;
        for (const auto& path : std::filesystem::directory_iterator(source / fileName)) {
            std::string fn = path.path().lexically_relative(source / fileName);
            if (std::find(ignoreFiles.begin(), ignoreFiles.end(), fn) != ignoreFiles.end()) { 
                spdlog::debug("{} is ignored and will not be linked", fn);
                continue; 
            }

            auto newVec = recursiveLink(source / fileName, dest / fileName, fn, safeDirNames, ignoreFiles);
            result.insert(result.end(), newVec.begin(), newVec.end());
        }
        return result;
    } else {
        if (std::find(ignoreFiles.begin(), ignoreFiles.end(), fileName) != ignoreFiles.end()) { 
            spdlog::debug("{} is ignored and will not be linked", fileName);
            return {};
        }

        return {{source / fileName, dest / fileName}};
    }
}
// }}}

}
