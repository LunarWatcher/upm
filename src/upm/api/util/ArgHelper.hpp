#pragma once

#include "lua.hpp"
#include <map>
#include <string>
#include <optional>
#include <typeindex>
#include <variant>

namespace upm::ArgHelper {
using LuaField = std::variant<std::string, long long, double, bool>;

extern std::map<std::string, std::optional<LuaField>> parseTable(lua_State* state, int idx, const std::map<std::string, std::type_index>& fields);

}
