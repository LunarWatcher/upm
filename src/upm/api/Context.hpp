#pragma once

#include <lua.hpp>

extern "C" {

extern int context_index(lua_State* state);
extern int context_test(lua_State* state);

extern int luaopen_context(lua_State* state);

}
