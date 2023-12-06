#!/usr/bin/bash

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
git clone --recursive https://github.com/LunarWatcher/upm /tmp/upm/_bootstrap
if [ $? -ne 0 ]; then
    echo "Failed to clone. If it already exists, remove /tmp/upm/_sources and try again."
    echo "Also note that you can update upm from within itself. This script is not meant for updates."
    exit -1
fi

mkdir /tmp/upm/
cd /tmp/upm/_bootstrap

echo "Checkout build directory"
mkdir -p build && cd build
# Bootstrap build
cmake .. -DCMAKE_BUILD_TYPE=Release && make -j $(nproc)

sudo ./bin/upm install upm@nightly --install_self=true
