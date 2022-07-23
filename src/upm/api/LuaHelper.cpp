#include "LuaHelper.hpp"

#include "Network.hpp"
#include "Exec.hpp"
#include "Filesystem.hpp"

#include <iostream>

namespace upm {

LuaHelper::LuaHelper() {
    state = luaL_newstate();
    // Load stdlib
    luaL_openlibs(state);
    // Load custom library functions
    registerLibrary("upmnetwork", luaopen_upmnetwork);
    registerLibrary("upmexec", luaopen_upmexec);
    registerLibrary("upmfs", luaopen_upmfilesystem);

    if (luaL_dofile(state, "lua/vim.lua") != 0) {
        std::cerr << "Failed to load lua/vim.lua: " << lua_tostring(state, -1) << std::endl;
        throw std::runtime_error("exec failed");
    }
}

LuaHelper::~LuaHelper() {
    lua_close(state);
}

void LuaHelper::registerLibrary(const char* libname, lua_CFunction func) {
    // I'm not sure why this is necessary
    luaL_requiref(state, libname, func, 1);
    lua_pop(state, 1);
}

}
