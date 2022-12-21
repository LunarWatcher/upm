#include "catch2/catch_test_macros.hpp"

#include "upm/api/LuaHelper.hpp"
#include "upm/api/json/JSON.hpp"

TEST_CASE("Ensure parsing works") {
    upm::LuaHelper helper;
    helper.init();
    helper.runString(R"(
         local j = require "json"

         local parseRes = j.parse('{"a": [1, 2, 3], "b": 6, "c": "여우"}');

         a = parseRes["a"];
         aSize = #(a);
         b = parseRes["b"];
         c = parseRes["c"];
    )");
    lua_getglobal(helper.getState(), "a");
    for (int i = 1; i <= 3; ++i) {
        lua_rawgeti(helper.getState(), -1, i);
        REQUIRE(lua_tointeger(helper.getState(), -1) == i);
        // Ensure the array is at the top of the stack
        lua_pop(helper.getState(), 1);
    }
    
    lua_getglobal(helper.getState(), "aSize");
    auto aSize = lua_tointeger(helper.getState(), -1);
    REQUIRE(aSize == 3);
    lua_getglobal(helper.getState(), "b");
    lua_getglobal(helper.getState(), "c");
    auto c = std::string(lua_tostring(helper.getState(), -1));
    REQUIRE(c == "여우");
}
