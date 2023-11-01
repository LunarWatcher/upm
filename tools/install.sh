#!/bin/bash

CMAKE_VERSION=$(cmake --version)

echo "Testing cmake..."
if [ $? -ne 0 ]; then
    echo "Install CMake and try again"
    exit -1
elif [[ "$CMAKE_VERSION" =~ "cmake version ([3-9]\.([6-9]|[1-9][0-9]+)\.[0-9]+|([12][0-9]+|[4-9][0-9]*))" ]]; then
    echo "Found CMake, but it doesn't appear to be a supported version. Please install CMake 3.10.0 or newer and try again"
    exit -1
fi
echo "$CMAKE_VERSION found and OK"
echo "Preparing clone directory"
sudo mkdir -p /opt/upm
echo "Cloning repository..."
git clone --recursive https://github.com/LunarWatcher/upm /opt/upm/_sources
if [ $? -ne 0 ]; then
    echo "Failed to clone. If it already exists, remove /opt/upm/_sources and try again."
    echo "Also note that you can update upm from within itself. This script is not meant for updates."
    exit -1
fi

cd /opt/upm/_sources

echo "Checkout build directory"
mkdir -p build && cd build
# Bootstrap build
cmake .. -DCMAKE_BUILD_TYPE=Release && make -j $(nproc)
# Make upm install itself
./bin/upm install upm@nightly
