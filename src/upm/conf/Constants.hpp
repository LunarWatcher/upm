#pragma once

#include <memory>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "stc/FS.hpp"

namespace upm::Constants {
static inline std::vector<spdlog::sink_ptr> sinks = {
    std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
    //std::make_shared<spdlog::sinks::basic_file_sink_mt>("/var/log/upm.log"),
};

const static auto inline logger = std::make_shared<spdlog::logger>("upm", sinks.begin(), sinks.end());

const static fs::path UPM_ROOT("/opt/upm/");
const static fs::path APPLY_ROOT("/usr/local/");

}
