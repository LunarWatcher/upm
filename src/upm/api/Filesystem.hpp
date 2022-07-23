#pragma once

#include <lua.hpp>
#include <string>

#include "upm/conf/Constants.hpp"

namespace upm {
namespace filesystem {
const auto logger = std::make_shared<spdlog::logger>("upm:filesystem", Constants::sinks.begin(), Constants::sinks.end());
}
}

extern "C" {

extern int upmfilesystem_exists(lua_State* state);
extern int upmfilesystem_sharedLibInstalled(lua_State* state);

extern int luaopen_upmfilesystem(lua_State* state);

}
