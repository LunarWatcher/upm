set (SOURCE_FILES
    upm/Context.cpp

    upm/conf/Config.cpp

    upm/package/PackageResolver.cpp
    upm/util/StrUtil.cpp
    upm/util/Version.cpp
    upm/conf/Config.cpp

    # Lua API {{{
    upm/api/LuaHelper.cpp
    upm/api/Network.cpp
    # }}}
)

add_library(upm_sources ${SOURCE_FILES})
target_include_directories(upm_sources PUBLIC ./ ../stc/src)
target_include_directories(upm_sources PUBLIC ${CONAN_INCLUDE_DIRS})

target_link_libraries(upm_sources ${CONAN_LIBS})

add_executable(upm upm/Main.cpp)
target_link_libraries(upm upm_sources)

# vim:ft=cmake
