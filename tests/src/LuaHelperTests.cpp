#include "upm/api/LuaHelper.hpp"
#include "upm/api/util/ArgHelper.hpp"
#include <catch2/catch_test_macros.hpp>
#include <lua.hpp>

TEST_CASE("It's possible to correctly convert arrays") {
    upm::LuaHelper helper;
    helper.init();
    helper.runString(R"(
        truth = {
            "trans", "rights", "are", "human", "rights"
        }
    )");
    lua_getglobal(helper.getState(), "truth");

    REQUIRE(lua_istable(helper.getState(), 1));
    std::vector<std::string> normalised;

    SECTION("parseList") {
        auto result = upm::ArgHelper::parseList(*helper, 1);
        REQUIRE(result.size() == 5);
        normalised.reserve(result.size());
        for (auto& it : result) {
            normalised.push_back(std::get<std::string>(it));
        }
    }
    SECTION("parseStringList") {
        normalised = upm::ArgHelper::parseStringList(*helper, 1);
        REQUIRE(normalised.size() == 5);
    }
    REQUIRE(normalised == std::vector<std::string>{"trans", "rights", "are", "human", "rights"});

    INFO("If this fails, the stack has drifted");
    REQUIRE(lua_istable(helper.getState(), -1));
}
