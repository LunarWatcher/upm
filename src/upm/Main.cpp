#include <algorithm>
#include <iostream>

#include "platform/Platform.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>

#include "Context.hpp"

// TODO: merge into stc (haven't I already?)

int main(int argc, const char* argv[]) {
    if (!fs::exists("/opt/upm")) {
        spdlog::error("/opt/upm does not exist. Please install properly. Note that the permissions aren't checked, but your user has to chown /opt/upm/");
        spdlog::error("If you're getting this and have no idea what you're doing, look at (or use) the tools/install.sh script.");
        return -1;
    }
    spdlog::set_pattern("[%^%l%$] %v");
    spdlog::cfg::load_env_levels();

    std::vector<std::string> arguments;
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == std::string("--debug")) {
            spdlog::set_level(spdlog::level::debug);
        } else {
            arguments.push_back(argv[i]);
        }
    }
    if (arguments.size() == 0) {
        arguments.push_back("help");
    }

    upm::Context ctx(arguments);
    try {
        return ctx.run();
    } catch (const std::exception& e) {
        spdlog::error("Aborting with message: {}", e.what());
    } catch(const std::string& e) {
        spdlog::error("Aborting with message: {}", e);
    } catch(const char* e) {
        spdlog::error("Aborting with message: {}", e);
    }
}
