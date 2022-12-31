#pragma once

#include <vector>
#include <string>
#include <stc/Environment.hpp>

namespace upm {

namespace VersionResolvers {

/**
 * Version resolver for tagged Git repositories
 */
std::string git(const std::string& repoPath);

}

}
