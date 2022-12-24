#pragma once

#include "stc/FS.hpp"

namespace upm {

/**
 * returns whether or not a path is a symlink, and points to a upm directory.
 */
inline bool isGoodSymlink(const fs::path& link) {
    return fs::is_symlink(link)
        && fs::read_symlink(link).string().find("/upm") != std::string::npos; 
}

}
