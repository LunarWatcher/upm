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
    upm/api/Exec.cpp
    # }}}
)

add_library(upm_sources ${SOURCE_FILES})
target_include_directories(upm_sources PUBLIC ./ ../stc/src)
target_include_directories(upm_sources PUBLIC ${CONAN_INCLUDE_DIRS})

target_link_libraries(upm_sources ${CONAN_LIBS})

add_executable(upm upm/Main.cpp)
target_link_libraries(upm upm_sources)

add_custom_target(luascripts
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lua
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/lua ${CMAKE_BINARY_DIR}/lua
    COMMENT "Copying ${CMAKE_SOURCE_DIR}/lua to ${CMAKE_BINARY_DIR}/lua"
)

add_dependencies(upm luascripts)

# vim:ft=cmake
