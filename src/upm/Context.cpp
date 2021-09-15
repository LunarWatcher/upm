#include "Context.hpp"

#include <iostream>

#include "InstallationResolver.hpp"
#include "vm/VersionManager.hpp"

// not even sure if this matters, I'm sure there's an include somewhere else
// that fails the Windows build, that's built earlier in the translation unit.
// Maybe
// Idk
#ifdef _WIN32
#error "Windows isn't supported because of a lack of a universal binary dir."
#else
#include <unistd.h>
#endif

namespace upm {

Context::Context(const std::vector<std::string>& cmd) : input(cmd), isRoot(!getuid()) {
}

int Context::run() {
    std::string command = input[0];
    input.erase(input.begin());

    
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

# Upgrading versioned packages
Dependent on issues #1, #4, #6 (read: TBD)

# Updating upm
)" << std::endl;
    } else if (command == "install") {
        if (input.size() < 1) {
            std::cerr << "What package?" << std::endl;
            return -1;
        }
        if (!isRoot) {
            std::cout << "Please run upm as sudo to install this package.\n"
                << "If you meant to install it for your user, remember to pass --local"
                << std::endl;
            return -1;
        }
        resolve(input[0], *this);
    } else if (command == "apply") {
        if (input.size() < 1) {
            std::cerr << "What package?" << std::endl;
            return -1;
        }
        if (!isRoot) {
            std::cout << "Please run upm as sudo to install this package.\n"
                << "If you meant to install it for your user, remember to pass --local"
                << std::endl;
            return -1;
        }
        enable(input[0], *this);
    } else {
        // Commands part of the help, but that aren't implemented yet are still unknown.
        // (read: they're not bugs, for the record :) )
        std::cerr << "Unknown command: " << command << std::endl;
        return -1;
    }

    return 0;
}

}
