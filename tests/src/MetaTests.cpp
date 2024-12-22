#include <catch2/catch_test_macros.hpp>

#include <upm/util/syscompat/Nproc.hpp>

#ifdef TEST_THREADS
TEST_CASE("Thread count detection", "[Meta][Multithreading]") {
    REQUIRE(upm::util::getHardwareConcurrency() == TEST_THREADS);
}
#endif
