#include "VersionResolvers.hpp"

extern "C" {

int verresolvers_git(lua_State* git) {
    return 0;
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
