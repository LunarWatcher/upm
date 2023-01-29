#include "Activators.hpp"

#include "upm/package/Activators.hpp"

extern "C" {

int activators_universalUNIX(lua_State* state) {
    if (!upm::Activators::recursiveUniversalUNIXLink()) {
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
