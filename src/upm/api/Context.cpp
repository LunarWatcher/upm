// I regret these filenames
#include "Context.hpp"
#include "Constants.hpp"
#include "upm/Context.hpp"
#include "LuaHelper.hpp"

#include <iostream>


extern "C" {

static const luaL_Reg contextMetatable[] = {
    {"getArch", context_getArch},
    // Meta methods
    {"__index", context_index},
    {nullptr, nullptr}
};


int context_index(lua_State* state) {

    upm::Context** data = static_cast<upm::Context**>(luaL_checkudata(state, 1, MT_Context));

    std::string name = luaL_checklstring(state, 2, nullptr);

    if (name == "package") {
        lua_pushstring(state, (*data)->package.c_str());
        return 1;
    } else if (name == "version") {
        
        lua_pushstring(state, (*data)->packageVersion.c_str());
        return 1;
    } else {
        // Why the fuck can I not use rawgetp?

        luaL_getmetatable(state, MT_Context);
        lua_pushvalue(state, 2);
        lua_rawget(state, -2);
    }
    return 1;
}

int context_getArch(lua_State* state) {
    upm::Context** data = static_cast<upm::Context**>(luaL_checkudata(state, 1, MT_Context));

    lua_pushstring(state, (*data)->sysInfo.os.c_str());
    lua_pushstring(state, (*data)->sysInfo.cpuArch.c_str());
    return 2;
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
