set (SOURCE_FILES
    upm/Context.cpp

    upm/conf/Config.cpp

    upm/package/Activators.cpp
    upm/package/VersionResolvers.cpp
    upm/util/Version.cpp
    upm/conf/Config.cpp

    upm/input/Parser.cpp

    # Lua API {{{
    upm/api/LuaHelper.cpp
    upm/api/Network.cpp
    upm/api/Exec.cpp
    upm/api/Filesystem.cpp
    upm/api/Context.cpp
    upm/api/Git.cpp

    upm/api/stdfunc/Activators.cpp
    upm/api/stdfunc/VersionResolvers.cpp

    upm/api/json/JSON.cpp

    upm/api/io/LogAPI.cpp
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
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lua/upm
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/lua ${CMAKE_BINARY_DIR}/lua/upm
    COMMENT "Copying ${CMAKE_SOURCE_DIR}/lua to ${CMAKE_BINARY_DIR}/lua/upm"
)

add_dependencies(upm luascripts)

install(TARGETS upm DESTINATION bin)
install(DIRECTORY ${CMAKE_BINARY_DIR}/lua DESTINATION .)

# vim:ft=cmake
