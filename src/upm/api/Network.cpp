#include "Network.hpp"

#include <iostream>

// Do I really need this twice?
extern "C" {

int upmnetwork_test(lua_State* state) {
    std::cout << "Look at me, I'm a moving target!\n";
    return 0;
}

int luaopen_upmnetwork(lua_State* state) {

    static const luaL_Reg functions[] = {
        {"test", upmnetwork_test},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);

    return 1;
}

}
