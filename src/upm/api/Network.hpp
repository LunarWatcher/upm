#pragma once

#include <lua.hpp>

#include "upm/conf/Constants.hpp"

namespace upm { namespace network {
const auto logger = std::make_shared<spdlog::logger>("upm:network", Constants::sinks.begin(), Constants::sinks.end());
}}

// externs are required for name mangling and All That Fun Crap:tm:
// Can I use namespaces with this though?
extern "C" {

extern int upmnetwork_request(lua_State* state);
extern int upmnetwork_gitClone(lua_State* state);

// Who in the actual fuck decided lua_State was a good naming practice?
// Like, preferences and shit aside, this name is blatantly disgusting
extern int luaopen_upmnetwork(lua_State* state);

}
