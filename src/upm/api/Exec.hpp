#pragma once

#include <lua.hpp>

#include "upm/conf/Constants.hpp"

namespace upm {
namespace exec {
const auto logger = std::make_shared<spdlog::logger>("upm:exec", Constants::sinks.begin(), Constants::sinks.end());
}
}

extern "C" {

extern int upmexec_exec(lua_State* state);

extern int luaopen_upmexec(lua_State* state);

}
