#pragma once

#include <lua.hpp>

extern "C" {

extern int verresolvers_git(lua_State* state);

extern int luaopen_verresolvers(lua_State* state);

}
