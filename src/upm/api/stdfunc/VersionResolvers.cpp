#include "VersionResolvers.hpp"

#include "upm/package/VersionResolvers.hpp"

extern "C" {

int verresolvers_git(lua_State* state) {
    lua_pushstring(state,
        upm::VersionResolvers::git(
            luaL_checkstring(state, 1)           
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
