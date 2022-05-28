#include "upm/util/Version.hpp"

#include "catch2/catch_test_macros.hpp"

TEST_CASE("Make sure comparisons work like they're meant to", "[TestComparison]") {
    upm::Version baseline("4.2.9");
    upm::Version other("4.2.9");
    REQUIRE(baseline == upm::Version{"4.2.9"}); // verify common sense
    REQUIRE(!(baseline < other));
    REQUIRE(!(baseline > other));

    other = upm::Version("4.3.0");

    REQUIRE(baseline < other);
    REQUIRE(other > baseline);
}

TEST_CASE("Test a few typical edge-cases", "[EdgeCaseComparison]") {
    upm::Version three("3.0"), three1("3.1");
    // test Python edge-case
    upm::Version ten("3.10"), nine("3.9.7");

    REQUIRE(three < three1);

    // Make sure 3.9 > 3.0 and 3.1
    REQUIRE(nine > three);
    REQUIRE(nine > three1);

    // Make sure 3.10 > 3.0, 3.1, and 3.9.
    // If 3.10 < 3.9, it's sorted incorrectly, like the Python ftp server is: https://www.python.org/ftp/python/
    REQUIRE(ten > three);
    REQUIRE(ten > three1);
    REQUIRE(ten > nine);

    upm::Version ten0("3.10.0"), ten1("3.10.1");
    REQUIRE (ten0 > nine);
    REQUIRE(ten == ten0);

    REQUIRE(ten1 > ten0);
    REQUIRE(ten1 > ten);
}

TEST_CASE("Test major versions", "[MajorVersionComparison]") {
    upm::Version three("3.10.0"), four("4.0");
    REQUIRE(three < four);
    REQUIRE(three != four);
    REQUIRE(four > three);
    REQUIRE_FALSE(three > four);

    upm::Version two("2.10");
    REQUIRE(two < three);
    REQUIRE(two < four);

    REQUIRE(four > three);
    REQUIRE(four > two);

    REQUIRE_FALSE(two > four);
}

TEST_CASE("Ensure proper checks", "[Version]") {
    // This test case makes sure there isn't bad checks.
    // Bug noticed when adding Python, where a > b, b < a, but b > a as well.
    // This obviously doesn't make sense. A few other tests have
    // been amended to add this test case, but
    // this test is exclusively for that.
    upm::Version a("2.9.13"),
            b("2.9.2");
    REQUIRE(a > b);
    REQUIRE(b < a);
    REQUIRE_FALSE(b > a);
}
