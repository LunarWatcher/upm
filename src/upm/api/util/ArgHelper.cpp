#include "ArgHelper.hpp"
#include "lua.hpp"
#include <optional>
#include <stdexcept>
#include <vector>

namespace upm {

std::map<std::string, std::optional<ArgHelper::LuaField>> ArgHelper::parseTable(
    lua_State* state,
    int idx,
    const std::map<std::string, std::type_index>& fields
) {
    if (state == nullptr) {
        throw std::runtime_error("Bad girl");
    }
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
    if (state == nullptr) {
        throw std::runtime_error("Bad girl");
    }

    auto length = luaL_len(state, idx);

    std::vector<ArgHelper::LuaField> out{};
    out.reserve(length);
    for (lua_Integer i = 1; i <= length; ++i) {
        lua_rawgeti(state, idx, i); // +1 on the stack

        if (lua_isstring(state, -1)) {
            size_t len;
            auto cstr = luaL_checklstring(state, -1, &len);
            out.push_back(std::string(cstr, len));
        } else if (lua_isinteger(state, -1)) {
            // Context: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=114592 - this does admittedly say it's a GCC14 bug
            // (if I'm reading the bug tracker right), but it's the _exact_ same situation. Just ignore it for now
            out.push_back(luaL_checkinteger(state, -1)); // This produces a bogus -Wmaybe-uninitialized warning under GCC13. It can be ignored
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
