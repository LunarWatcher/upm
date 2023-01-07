#include "Config.hpp"

#include "stc/Environment.hpp"
#include "upm/Context.hpp"

#include <spdlog/spdlog.h>
#include <fstream>

namespace upm {

Config::Config(Context* ctx) : ctx(ctx) {
    confPath = "/opt/upm/.upmrc";

    if (!fs::exists(confPath)) {
        spdlog::debug("No config found at " + confPath.string());
        return;
    }

    spdlog::debug("Loading config");
    std::ifstream cInp(confPath);
    cInp >> data;

}

Config::~Config() {
    save();
}

void Config::save() {
    if (this->confPath.empty() || fs::is_directory(this->confPath)) {
        spdlog::warn("No confPath set; cannot save");
        return;
    }
    std::ofstream cOut(this->confPath);
    cOut << data;
}

void Config::setup() {
    // reserved for future use
}

}
