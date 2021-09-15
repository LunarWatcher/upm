#include <algorithm>
#include <iostream>

#include "Context.hpp"

std::string toLower(std::string in) {
    std::transform(in.begin(), in.end(), in.begin(),
        [](const char& c) {
            return std::tolower(c);
        });
    return in;
}

int main(int argc, const char* argv[]) {
    std::vector<std::string> arguments;
    if (argc == 1) arguments.push_back("help");
    else {
        for (int i = 1; i < argc; ++i) {
            arguments.push_back(argv[i]);
        }
    }

    upm::Context ctx(arguments);
    return ctx.run();
}
