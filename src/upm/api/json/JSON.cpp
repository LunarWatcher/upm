#include "JSON.hpp"

#include "upm/api/Constants.hpp"
#include "upm/api/LuaHelper.hpp"

#include <iostream>

extern "C" {

void upmjson_parseInternal(lua_State* state, const nlohmann::json& json) {
    if (json.is_null()) {
        lua_pushnil(state);
    } else if (json.is_number_integer()) {
        lua_pushinteger(state, json.get<long long>());
    } else if (json.is_number_float()) {
        lua_pushnumber(state, json.get<double>());
    } else if (json.is_string()) {
        lua_pushstring(state, json.get<std::string>().c_str());
    } else if (json.is_boolean()) {
        lua_pushboolean(state, json.get<bool>());
    } else if (json.is_array()) {
        lua_newtable(state);

        int index = 0;
        for (auto& value : json) {
            upmjson_parseInternal(state, value);
            lua_seti(state, -2, ++index);
        }
    } else if (json.is_object()) {
        lua_newtable(state);

        for (auto& [k, v] : json.items()) {
            upmjson_parseInternal(state, v);
            lua_setfield(state, -2, k.c_str()); // step[k] = v
        }
    } else {
        throw std::runtime_error("bad JSON parsing at " + json.dump());
    }
}

int upmjson_parse(lua_State* state) {
    auto json = nlohmann::json::parse(luaL_checkstring(state, 1));
    upmjson_parseInternal(state, json);

    return 1;
}

int luaopen_upmjson(lua_State* state) {
    static const luaL_Reg functions[] {
        {"parse", upmjson_parse},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);

    return 1;
}

}
