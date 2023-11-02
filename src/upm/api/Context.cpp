// I regret these filenames
#include "Context.hpp"
#include "Constants.hpp"
#include "lua.h"
#include "upm/Context.hpp"
#include "LuaHelper.hpp"

#include <iostream>


extern "C" {

static const luaL_Reg contextMetatable[] = {
    {"getArch", context_getArch},
    {"checkInstalled", context_checkInstalled},
    // Meta methods
    {"__index", context_index},
    {"__newindex", context_newindex},
    {nullptr, nullptr}
};


int context_index(lua_State* state) {

    upm::Context** data = static_cast<upm::Context**>(luaL_checkudata(state, 1, MT_Context));

    std::string name = luaL_checklstring(state, 2, nullptr);

    if (name == "package") {
        lua_pushstring(state, (*data)->package.c_str());
    } else if (name == "version") {
        lua_pushstring(state, (*data)->packageVersion.c_str());
    } else if (name == "resolvedVersion") {
        lua_pushstring(state, (*data)->resolvedPackageVersion.c_str());
    } else if (name == "prefix") {
        lua_pushstring(state, (*data)->getPrefix().c_str());
    } else if (name == "options") {
        lua_newtable(state);

        for (auto& [flag, value] : (**data).flags) {
            lua_pushstring(state, value.c_str());
            lua_setfield(state, -2, flag.c_str());
        }
    } else {
        // Note for future self: this is to allow function resolution from the metatable.
        luaL_getmetatable(state, MT_Context);
        lua_pushvalue(state, 2);
        lua_rawget(state, -2);
    }

    // __index seems to only allow a single return value, so no point in returning anything else.
    return 1;
}

int context_newindex(lua_State* state) {
    upm::Context** data = static_cast<upm::Context**>(luaL_checkudata(state, 1, MT_Context));
    std::string key = luaL_checkstring(state, 2);
    if (key == "resolvedVersion") {
        (*data)->resolvedPackageVersion = luaL_checkstring(state, 3);
        return 0;
    } else {
        return luaL_error(state, "Nope");
    }
}

int context_getArch(lua_State* state) {
    upm::Context** data = static_cast<upm::Context**>(luaL_checkudata(state, 1, MT_Context));

    lua_pushstring(state, (*data)->sysInfo.os.c_str());
    lua_pushstring(state, (*data)->sysInfo.cpuArch.c_str());

    return 2;
}

int context_checkInstalled(lua_State* state) {
    upm::Context** data = static_cast<upm::Context**>(luaL_checkudata(state, 1, MT_Context));
    if ((*data)->checkInstalled()) {
        return luaL_error(state, "Package already installed. Pass --reinstall to automatically uninstall and reinstall.");
    }

    return 0;
}

int luaopen_context(lua_State* state) {

    luaL_newmetatable(state, MT_Context);
    luaL_setfuncs(state, contextMetatable, 0);
    lua_pop(state, 1);

    //luaL_newlib(state, functions);

    auto udata = (upm::Context**) lua_newuserdata(state, sizeof(upm::Context*));
    *udata = upm::Context::inst;

    luaL_setmetatable(state, MT_Context);
    lua_setglobal(state, CONTEXT_IDENTIFIER);

    return 1;
}

}
