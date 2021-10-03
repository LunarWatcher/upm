#pragma once

#include "upm/conf/Config.hpp"
#include <vector>
#include <string>


namespace upm {

class Context {
private:
    std::vector<std::string> input;
public:
    std::string package;
    std::string packageVersion;

    const bool isRoot;
    Config cfg;

    Context(const std::vector<std::string>& cmd);

    void resolvePackageContext(const std::string& package);

    int run();

    const decltype(input)& getInput() { return input; }
};

}
