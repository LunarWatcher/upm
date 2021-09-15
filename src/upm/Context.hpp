#pragma once

#include "upm/conf/Config.hpp"
#include <vector>
#include <string>


namespace upm {

class Context {
private:
    std::vector<std::string> input;
    Config cfg;

    bool isRoot;
public:
    Context(const std::vector<std::string>& cmd);

    int run();

    const decltype(input)& getInput() { return input; }
};

}
