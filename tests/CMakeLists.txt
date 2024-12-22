add_executable(tests
    src/JSONAPITest.cpp
    src/TestRunner.cpp
    src/VersionTest.cpp

    src/MetaTests.cpp
)
Include(FetchContent)

if (TEST_THREADS)
    target_compile_definitions(tests PRIVATE -DTEST_THREADS=${TEST_THREADS})
endif()

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(tests Catch2::Catch2 upm_sources)
