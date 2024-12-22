#pragma once

#include <filesystem>

namespace upm {

/**
 * returns whether or not a path is a symlink, and points to a upm directory.
 */
inline bool isGoodSymlink(const std::filesystem::path& link) {
    return std::filesystem::is_symlink(link)
        && std::filesystem::read_symlink(link).string().find("/upm") != std::string::npos; 
}

}
