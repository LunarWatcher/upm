#include "catch2/catch_test_macros.hpp"

#include "upm/api/LuaHelper.hpp"
#include "upm/api/json/JSON.hpp"

TEST_CASE("Ensure parsing works", "[Feat][Json]") {
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

TEST_CASE("Ensure nesting works", "[Feat][Json]") {
    upm::LuaHelper helper;
    helper.init();
    helper.runString(R"(
        local j = require "json"

        local parseRes = j.parse('{"a": [1, 2, 3], "b": {"b1": {"b2": [1, 2, {"b3": 69}]}}, "c": "여우"}');
        
        b3 = parseRes["b"]["b1"]["b2"][3]["b3"];
        
    )");

    lua_getglobal(helper.getState(), "b3");
    auto b3 = lua_tointeger(helper.getState(), -1);
    REQUIRE(b3 == 69);
}

TEST_CASE("Ensure primitives work", "[Feat][Json]") {
    upm::LuaHelper helper;
    helper.init();
    helper.runString(R"(
        local j = require "json"

        res = j.parse('"never gonna give you up"');
        
    )");
    lua_getglobal(helper.getState(), "res");
    REQUIRE(strcmp(lua_tostring(helper.getState(), -1), "never gonna give you up") == 0);

    REQUIRE_THROWS_AS(helper.runString(R"(local j = require "json"; j.parse('6, 9'))"), nlohmann::json::parse_error);
    REQUIRE_THROWS_AS(helper.runString(R"(local j = require "json"; j.parse('6 9'))"), nlohmann::json::parse_error);
    REQUIRE_THROWS_AS(helper.runString(R"(local j = require "json"; j.parse('6 "abcd"'))"), nlohmann::json::parse_error);

    helper.runString("local j = require \"json\"; local pr = j.parse('6.9'); res = pr == 6.9;");
    lua_getglobal(helper.getState(), "res");
    REQUIRE(lua_toboolean(helper.getState(), -1));
}

