#pragma once

#include "nlohmann/json.hpp"

#include <lua.hpp>

extern "C" {

extern void upmjson_parseInternal(lua_State* state, const nlohmann::json& json);
extern int upmjson_parse(lua_State* state);

extern int luaopen_upmjson(lua_State* state);
}
