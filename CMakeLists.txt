cmake_minimum_required(VERSION 3.6)
project(upm)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (NOT CMAKE_BUILD_TYPE)
    # Make explicit flags redundant for release builds
    set(CMAKE_BUILD_TYPE Release)
endif()
if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUPM_DEBUG=1")
endif()

set (CMAKE_CXX_STANDARD 20)

if(USE_LIBCPP)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()

# Required to preserve lookup stuff
list(APPEND CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
#list(APPEND CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(SPDLOG_FMT_EXTERNAL OFF CACHE STRING "" FORCE)
set(SPDLOG_USE_STD_FORMAT ON CACHE STRING "" FORCE)
set(LUA_TAG v5.4.6 CACHE STRING "" FORCE)
set(FMT_INSTALL OFF CACHE STRING "" FORCE)
set(SPDLOG_INSTALL OFF CACHE STRING "" FORCE)
set(CURL_INSTALL OFF CACHE STRING "" FORCE)
set(CPR_USE_SYSTEM_CURL ON CACHE STRING "" FORCE)


include(FetchContent)
FetchContent_Declare(stc
    GIT_REPOSITORY https://github.com/LunarWatcher/stc
)
FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.14.1
)
FetchContent_Declare(nlohmann
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.16.0
)
FetchContent_Declare(lua-cmake
    GIT_REPOSITORY https://github.com/LunarWatcher/lua-cmake/
)

FetchContent_MakeAvailable(stc)
FetchContent_MakeAvailable(nlohmann)
FetchContent_MakeAvailable(spdlog)
FetchContent_MakeAvailable(lua-cmake)
FetchContent_GetProperties(cpr)
if (NOT cpr_POPULATED)
    FetchContent_Populate(cpr)
    add_subdirectory(${cpr_SOURCE_DIR} ${cpr_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Versioning {{{
find_program(GIT git)
if (GIT)
    execute_process(COMMAND ${GIT} describe --tags --always
        OUTPUT_VARIABLE UPM_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Detected version: ${UPM_VERSION}")
else()
    message(WARNING "Git not found. Defaulting to unknown version. This will not have an effect on the program beyond a missing version")
    set(UPM_VERSION "VERSION UNKNOWN. Who doesn't have Git? It's the 2020s. Install it already")
endif()
# }}}
 
add_subdirectory(src)
add_subdirectory(tests EXCLUDE_FROM_ALL)

add_custom_target(run
    COMMAND upm
    DEPENDS upm
    COMMENT "Run upm")
add_custom_target(test
    COMMAND tests
    DEPENDS tests
    COMMENT "Test upm")

# vim:ft=cmake
