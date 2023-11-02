#pragma once

#include "lua.hpp"

extern "C" {
    
extern int activators_universalUNIX(lua_State* state);
extern int activators_activateSingle(lua_State* L);

extern int luaopen_activators(lua_State* state);

}
