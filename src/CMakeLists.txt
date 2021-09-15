set (SOURCE_FILES
    upm/Main.cpp
    upm/Context.cpp

    upm/conf/Config.cpp

    upm/package/PackageResolver.cpp
    upm/util/StrUtil.cpp
    upm/conf/Config.cpp
)

add_executable(upm ${SOURCE_FILES})
target_include_directories(upm PUBLIC ./ ../stc/src)
target_include_directories(upm PUBLIC ${CONAN_INCLUDE_DIRS})

target_link_libraries(upm ${CONAN_LIBS})

# vim:ft=cmake
