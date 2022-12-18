#pragma once

#include "upm/Context.hpp"
#include <map>
#include <string>
#include <vector>

namespace upm {

typedef struct {
    std::string name,
        version;
    VersionType type;
} PackageInput;

typedef struct {
    // packages<data<name, version, type>>
    std::vector<std::pair<std::string, std::string>> packages;
} ParserResult;

std::map<std::string, std::string> parse(const std::vector<std::string>& argv);

}
