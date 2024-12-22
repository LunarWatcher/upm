#include "ArgHelper.hpp"
#include "lua.h"
#include <optional>
#include <stdexcept>
#include <iostream>

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

}
