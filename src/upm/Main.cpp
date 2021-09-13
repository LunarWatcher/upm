#include <algorithm>
#include <iostream>

// not even sure if this matters, I'm sure there's an include somewhere else
// that fails the Windows build.
#ifdef _WIN32
#error "Windows isn't supported because of a lack of a universal binary dir."
#else
#include <unistd.h>
#endif

#include "InstallationResolver.hpp"
#include "VersionManager.hpp"

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
    apply               Applies a specific version of a package
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
        if (getuid()) {
            std::cout << "Please run upm as sudo to install this package.\n"
                << "If you meant to install it for your user, remember to pass --local"
                << std::endl;
            return -1;
        }
        upm::resolve(arguments[0]);
    } else if (command == "apply") {
        if (arguments.size() < 1) {
            std::cerr << "What package?" << std::endl;
            return -1;
        }
        if (getuid()) {
            std::cout << "Please run upm as sudo to install this package.\n"
                << "If you meant to install it for your user, remember to pass --local"
                << std::endl;
            return -1;
        }
        upm::enable(arguments[0]);
    }
}
