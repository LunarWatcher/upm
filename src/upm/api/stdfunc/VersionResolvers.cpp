#include "VersionResolvers.hpp"

#include "upm/package/VersionResolvers.hpp"

extern "C" {

int verresolvers_git(lua_State* state) {
    bool vPrefix = false;
    if (lua_gettop(state) >= 2) {
        vPrefix = (lua_toboolean(state, 2) != 0);
    }
    lua_pushstring(state,
        upm::VersionResolvers::git(
            luaL_checkstring(state, 1),
            vPrefix
        ).c_str()
    );
    return 1;
}

int luaopen_verresolvers(lua_State* state) {
    static const luaL_Reg functions[] = {
        {"git", verresolvers_git},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);
    return 1;
}

}
