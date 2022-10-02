// I regret these filenames
#include "Context.hpp"
#include "Constants.hpp"
#include "upm/Context.hpp"

#include <iostream>


extern "C" {

static const luaL_Reg contextMetatable[] = {
    {"test", context_test},
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

int context_test(lua_State* state) {
    lua_pushstring(state, "test object function");
    return 1;
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
