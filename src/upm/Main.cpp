#include <algorithm>
#include <iostream>

#ifdef _WIN32
#error "Windows isn't supported because of a lack of a universal binary dir."
#endif

#include "InstallationResolver.hpp"

std::string toLower(std::string in) {
    std::transform(in.begin(), in.end(), in.begin(),
        [](const char& c) {
            return std::tolower(c);
        });
    return in;
}

int main(int argc, const char* argv[]) {
    std::string command;
    std::vector<std::string> arguments;
    if (argc == 1) command = "help";
    else {
        for (int i = 1; i < argc; ++i) {
            if (i == 1) command = std::string(argv[i]);
            else arguments.push_back(argv[i]);
        }
    }

    if (command == "help") {
        std::cout << R"(Commands
    help                Shows this helpful message
    install             Installs one or more packages
    uninstall           Uninstalls one or more packages
    upgrade             Upgrades a package.
    list                Lists installed packages, along with installed versions.

Since upm allows multiple installed versions of certain programs,
install and uninstall may require a version to work.

Upgrading versioned packages // TODO: determine how upgrading and installing with versions is actually gonna work.
                             // (and yes, I know these aren't comments)
)" << std::endl;
    } else if (command == "install") {
        if (arguments.size() < 1) {
            std::cerr << "What package?" << std::endl;
            return -1;
        }
        upm::resolve(arguments[0]);
    }
}
