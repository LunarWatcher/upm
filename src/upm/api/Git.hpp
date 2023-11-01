#pragma once

#include <lua.hpp>

extern "C" {

extern int git_clone(lua_State* state);
extern int git_checkout(lua_State* state);
extern int git_pull(lua_State* state);
extern int git_fetch(lua_State* state);

extern int luaopen_git(lua_State* state);

}
