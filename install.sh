#!/bin/bash
# Make sure the virtualenv exists

CMAKE_VERSION=$(cmake --version)

echo "Testing cmake..."
if [ -z "$CMAKE_VERSION" -a "$CMAKE_VERSION" != " " ]; then
    echo "Install CMake and try again"
elif [[ "$CMAKE_VERSION" =~ "cmake version (3\.([6-9]|[1-9][0-9]+)\.[0-9]+|([12][0-9]+|[4-9][0-9]*))" ]]; then
    echo "Found CMake, but it doesn't appear to be a supported version. Please install CMake 3.10.0 or newer and try again"
    exit -1
fi

if [ ! -e env ]; then
    python3 -m venv env
fi
source ./env/bin/activate

echo "Making sure conan is good..."
python3 -m pip install --upgrade conan || exit -1

echo "Checkout build directory"
mkdir -p build && cd build
cmake .. && make
# TODO: make install
