set (SOURCE_FILES
    gamma/Main.cpp
    gamma/package/PackageResolver.cpp
    gamma/util/StrUtil.cpp
)

add_executable(projectgamma ${SOURCE_FILES})
target_include_directories(projectgamma PUBLIC ./)
target_include_directories(projectgamma PUBLIC ${CONAN_INCLUDE_DIRS})

target_link_libraries(projectgamma ${CONAN_LIBS})

# vim:ft=cmake
