#include "LuaHelper.hpp"
#include "Network.hpp"

#include <iostream>

namespace upm {

LuaHelper::LuaHelper() {
    state = luaL_newstate();
    // Load stdlib
    luaL_openlibs(state);
    // Load custom library functions
    registerLibrary("upmnetwork", luaopen_upmnetwork);

    //std::cout << "Lua: " << luaL_dofile(state, "test.lua") << std::endl;
}

LuaHelper::~LuaHelper() {
    lua_close(state);
}

void LuaHelper::registerLibrary(const char* libname, lua_CFunction func) {
    luaL_requiref(state, libname, func, 1);
    lua_pop(state, 1);
}

}
