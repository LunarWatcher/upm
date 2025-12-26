#include "ArgHelper.hpp"
#include "lauxlib.h"
#include "lua.h"
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

namespace upm {

std::map<std::string, std::optional<ArgHelper::LuaField>> ArgHelper::parseTable(lua_State* state, int idx, const std::map<std::string, std::type_index>& fields) {
    std::map<std::string, std::optional<LuaField>> res;

    for (const auto& [key, type] : fields) {
        std::optional<LuaField> field = std::nullopt;

        lua_getfield(state, idx, key.c_str());
        if (!lua_isnil(state, -1)) {

            if (type == typeid(long long)) {
                field = luaL_checkinteger(state, -1);
            } else if (type == typeid(bool)) {
                field = lua_toboolean(state, -1) == 1;
            } else if (type == typeid(double)) {
                field = luaL_checknumber(state, -1);
            } else if (type == typeid(std::string)) {
                field = luaL_checkstring(state, -1);
            } else {
                throw std::runtime_error("Programmer error: bad type supplied to parseTable");
            } 
        } 
        lua_pop(state, 1);
        res[key] = field;
    }

    return res;
}

std::vector<ArgHelper::LuaField> ArgHelper::parseList(lua_State* state, int idx) {
    auto length = luaL_len(state, idx);

    std::vector<ArgHelper::LuaField> out;
    for (lua_Integer i = 1; i <= length; ++i) {
        lua_rawgeti(state, idx, i); // +1 on the stack

        if (lua_isstring(state, -1)) {
            out.push_back(luaL_checkstring(state, -1));
        } else if (lua_isinteger(state, -1)) {
            out.push_back(luaL_checkinteger(state, -1));
        } else {
            throw std::runtime_error("Passed invalid type to list");
        }

        lua_pop(state, 1); // pop lua_rawgeti
    }

    return out;
}

// TODO: this function (but not the previous one) should be templated 
std::vector<std::string> ArgHelper::parseStringList(lua_State* state, int idx) {
    auto length = luaL_len(state, idx);

    std::vector<std::string> out;
    for (lua_Integer i = 1; i <= length; ++i) {
        lua_rawgeti(state, idx, i); // +1 on the stack
        // TODO: can this call work with meta methods too? rawgeti bypasses them, so this
        // function isn't fully universal

        if (lua_isstring(state, -1)) {
            out.push_back(luaL_checkstring(state, -1));
        } else {
            throw std::runtime_error("Passed invalid type to string array");
        }

        lua_pop(state, 1); // pop lua_rawgeti
    }

    return out;
}

}
