cmake_minimum_required(VERSION 3.10)
project(ProjectGamma)

set (CMAKE_CXX_STANDARD 20)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure (REQUIRES 
                        cpr/1.6.2
                        GENERATORS cmake)
conan_cmake_autodetect (settings)
conan_cmake_install (PATH_OR_REFERENCE . BUILD missing REMOTE conan-center SETTINGS ${settings})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src)
# vim:ft=cmake
