#include "upm/util/String.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Base escaping rules", "[Security][Shell]") {
    REQUIRE(upm::String::escapeShellArg("'") == "''\\'''");
    REQUIRE(upm::String::escapeShellArg("this is a string") == "'this is a string'");
    REQUIRE(upm::String::escapeShellArg("\"this is a string\"") == "'\"this is a string\"'");
    REQUIRE(upm::String::escapeShellArg("don't") == "'don'\\''t'");
}

TEST_CASE("Evil shell payloads", "[Security][Shell]") {
    std::vector<std::pair<std::string, std::string>> payloads = {
        {"'; rm -rf /; echo '", "''\\''; rm -rf /; echo '\\'''"},
    };

    for (auto& [src, goodEscape] : payloads) {
        INFO(src);
        REQUIRE(upm::String::escapeShellArg(src) == goodEscape);
    }
}

