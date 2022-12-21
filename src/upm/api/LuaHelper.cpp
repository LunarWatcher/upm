#include "LuaHelper.hpp"

#include "Network.hpp"
#include "Exec.hpp"
#include "Filesystem.hpp"
#include "Context.hpp"
#include "json/JSON.hpp"

#include <stc/FS.hpp>

#include <iostream>

namespace upm {

LuaHelper::LuaHelper() {
    // Need to make sure we have our own, scoped directory.
    // Means we don't have to deal with locks on shit made.
    // There can and still will be conflicts if upm runs concurrently,
    // but I'll eventually make a lock-based system for execution, similar
    // to how apt does it, to prevent this from happening.
    // Not sure how I'd go about it, however.
    fs::create_directory("/tmp/upm");

    state = luaL_newstate();
}

void LuaHelper::init() {
    // Load stdlib
    luaL_openlibs(state);
    // Load custom library functions
    registerLibrary("upmnetwork", luaopen_upmnetwork);
    registerLibrary("upmexec", luaopen_upmexec);
    registerLibrary("upmfs", luaopen_upmfilesystem);
    registerLibrary("context", luaopen_context);
    registerLibrary("json", luaopen_upmjson);

    // Init is called when the package is clear.

}

LuaHelper::~LuaHelper() {
    lua_close(state);
}

void LuaHelper::runFile(const std::string& fn) {
    // TODO: deal with cwd not containing lua
    // TODO(by extension): deal with lookup paths
    if (luaL_dofile(state, ("lua/" + fn).c_str()) != 0) {
        std::cerr << "Failed to load lua file: " << lua_tostring(state, -1) << std::endl;
        throw std::runtime_error("exec failed");
    }
}

void LuaHelper::runString(const std::string& script) {
    if (luaL_dostring(state, script.c_str())) {
        std::cerr << "Failed to run lua script: " << lua_tostring(state, -1) << std::endl;
        throw std::runtime_error("Exec failed");
    }
}

void LuaHelper::dump() {
    dump(state);
}

void LuaHelper::dump(lua_State* state) {
    for (int i = 1; i <= lua_gettop(state); i++) {
        std::cout << i << "\t" << luaL_typename(state, i) << "\t";
        switch (lua_type(state, i)) {
            case LUA_TNUMBER:
                std::cout << lua_tonumber(state, i) << std::endl;;
                break;
            case LUA_TSTRING:
                std::cout << lua_tostring(state, i) << std::endl;
                break;
            case LUA_TBOOLEAN:
                std::cout << (lua_toboolean(state, i) ? "true" : "false") << std::endl;
                break;
            case LUA_TNIL:
                std::cout << "nil" << std::endl;
                break;
            default:
                std::cout << lua_topointer(state, i) << std::endl;
                break;
        }
    }
}

void LuaHelper::install() {

}

void LuaHelper::registerLibrary(const char* libname, lua_CFunction func) {
    // I'm not sure why this is necessary
    luaL_requiref(state, libname, func, 1);
    lua_pop(state, 1);
}

}
