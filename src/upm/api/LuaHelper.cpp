#include "LuaHelper.hpp"

#include "Network.hpp"
#include "Filesystem.hpp"
#include "Context.hpp"
#include "Git.hpp"

#include "json/JSON.hpp"

#include "io/LogAPI.hpp"

#include "stdfunc/Activators.hpp"
#include "stdfunc/VersionResolvers.hpp"

#include <filesystem>

#include <iostream>

namespace upm {

LuaHelper::LuaHelper() {
    // Need to make sure we have our own, scoped directory.
    // Means we don't have to deal with locks on shit made.
    // There can and still will be conflicts if upm runs concurrently,
    // but I'll eventually make a lock-based system for execution, similar
    // to how apt does it, to prevent this from happening.
    // Not sure how I'd go about it, however.
    std::filesystem::create_directory("/tmp/upm");

    state = luaL_newstate();
}

void LuaHelper::init() {
    // Load stdlib
    luaL_openlibs(state);
    // Load custom library functions
    registerLibrary("upmnetwork", luaopen_upmnetwork);
    registerLibrary("upmfs", luaopen_upmfilesystem);
    registerLibrary("context", luaopen_context);
    registerLibrary("json", luaopen_upmjson);
    registerLibrary("upmlog", luaopen_upmlog);
    registerLibrary("activators", luaopen_activators);
    registerLibrary("vResolvers", luaopen_verresolvers);
    registerLibrary("git", luaopen_git);

    // Init is called when the package is clear.

}

LuaHelper::~LuaHelper() {
    lua_close(state);
}

void LuaHelper::loadPackage(const std::string& package, const std::string& fn) {
    if (luaCache.find(package) == luaCache.end()) {
        if (luaL_dofile(state, fn.c_str()) != 0) {
            std::cerr << "Failed to load lua file: " << lua_tostring(state, -1) << std::endl;
            throw std::runtime_error("exec failed");
        }
        auto r = luaL_ref(state, LUA_REGISTRYINDEX);
        if (r == LUA_REFNIL) {
            throw std::runtime_error("Unexpected arguments received");
        }
        luaCache[package] = r;
    } else {
        spdlog::debug("Loading functions for {} from cache", package);
    }
}

void LuaHelper::invoke(const std::string& package, const std::string& func) {
    lua_rawgeti(state, LUA_REGISTRYINDEX, luaCache.at(package));

    lua_getfield(state, -1, func.c_str());

    if (lua_isnil(state, -1)) {
        spdlog::error("{} does not define {}", package, func);
        throw std::runtime_error("Invalid command for file");
    }
    if (lua_pcall(state, 0, 0, 0) != LUA_OK) {
        if (lua_isstring(state, -1) != 0) {
            spdlog::error(lua_tostring(state, -1));
        } else {
            dump();
        }
        throw std::runtime_error("A critical Lua failure occurred.");
    }
    lua_pop(state, lua_gettop(state));
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
                std::cout << ((lua_toboolean(state, i) != 0) ? "true" : "false") << std::endl;
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
