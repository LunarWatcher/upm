#include "Git.hpp"

extern "C" {

int git_clone(lua_State* state) {
    
}

int git_checkout(lua_State* state) {

}

int git_pull(lua_State* state) {

}

int luaopen_git(lua_State* state) {
    static const luaL_Reg functions[] = {
        {"clone", git_clone},
        {"checkout", git_checkout},
        {"pull", git_pull},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);
    return 1;
}

}
