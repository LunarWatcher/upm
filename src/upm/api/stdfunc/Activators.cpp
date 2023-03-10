#include "Activators.hpp"

#include "lua.h"
#include "upm/package/Activators.hpp"

extern "C" {

int activators_universalUNIX(lua_State* state) {
    std::vector<std::string> safeDirNames;
    if (lua_gettop(state) >= 1) {
        auto tableIdx = lua_absindex(state, -1);
        luaL_checktype(state, tableIdx, LUA_TTABLE);

        lua_pushnil(state);

        while (lua_next(state, tableIdx) != 0) {
            std::string value = luaL_checkstring(state, -1);
            safeDirNames.push_back(value);
            lua_pop(state, 1);
        }

    }
    if (!upm::Activators::recursiveUniversalUNIXLink(safeDirNames)) {
        return luaL_error(state, "Activation function errored.");
    }
    return 0;
}

int luaopen_activators(lua_State* state) {
    static const luaL_Reg functions[] = {
        {"universalUNIX", activators_universalUNIX},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);

    return 1;
}

}
