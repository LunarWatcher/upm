add_executable(tests
    src/JSONAPITest.cpp
    src/TestRunner.cpp
    src/VersionTest.cpp
)
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.3.0)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(tests Catch2::Catch2 upm_sources)
