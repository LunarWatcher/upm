#include "Config.hpp"

#include "stc/Environment.hpp"
#include "upm/Context.hpp"

#include <spdlog/spdlog.h>
#include <fstream>

namespace upm {

Config::Config(Context* ctx) : ctx(ctx) {
    fs::path conf = ctx->isRoot ? "/opt/upm-bin/" : stc::getHome();
    if (!fs::exists(conf)) {
        fs::create_directories(conf);
    }
    conf /= ".upmrc";
    this->confPath = conf;
    if (!fs::exists(conf)) {
        spdlog::debug("No config found at " + conf.string());
        return;
    }

    spdlog::debug("Loading config");
    std::ifstream cInp(conf);
    cInp >> data;

}

Config::~Config() {
    if (this->confPath.empty() || fs::is_directory(this->confPath)) {
        spdlog::warn("No confPath set; cannot save");
        return;
    }
    std::ofstream cOut(this->confPath);
    cOut << data;

}

}
