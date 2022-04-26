#include "Network.hpp"
#include "lauxlib.h"
#include "lua.h"
#include "lua.hpp"

#include <iostream>
#include <cpr/cpr.h>

// Do I really need this twice?
extern "C" {

int upmnetwork_request(lua_State* state) {
    if (lua_gettop(state) < 1) {
        return luaL_error(state, "Need an argument");
    }

    std::string url = luaL_checklstring(state, 1, nullptr);


    auto response = cpr::Get(cpr::Url(url));

    lua_newtable(state);
    lua_pushstring(state, response.text.c_str());
    lua_setfield(state, 2, "text");

    return 1;
}

int luaopen_upmnetwork(lua_State* state) {

    static const luaL_Reg functions[] = {
        {"request", upmnetwork_request},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);

    return 1;
}

}
