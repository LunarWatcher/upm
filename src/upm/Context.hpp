#pragma once

#include "upm/conf/Config.hpp"
#include <vector>
#include <string>


namespace upm {

class Context {
private:
    std::vector<std::string> input;
public:

    const bool isRoot;
    Config cfg;

    Context(const std::vector<std::string>& cmd);

    int run();

    const decltype(input)& getInput() { return input; }
};

}
