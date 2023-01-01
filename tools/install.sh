#!/bin/bash

if [ "$EUID" -eq 0 ]; then
    echo "This install script cannot be run as root."
    exit -1
fi

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
sudo chown $USER -R /opt/upm/
echo "Cloning repository..."
git clone --recursive https://github.com/LunarWatcher/upm /opt/upm/_sources
if [ $? -ne 0 ]; then
    echo "Failed to clone. If it already exists, remove /opt/upm/_sources and try again."
    echo "Also note that you can update upm from within itself. This script is not meant for updates."
    exit -1
fi

echo "Clone successful. Entering repo..."
cd /opt/upm/_sources

if [ ! -e env ]; then
    python3 -m venv env
fi
source ./env/bin/activate

echo "Making sure conan is good..."
python3 -m pip install --upgrade conan || exit -1

echo "Checkout build directory"
mkdir -p build && cd build
# TODO: Hook up -j <threads> with automatic core detection
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/upm/packages/upm-nightly && make -j $(nproc)
make install -j $(nproc)

#TODO: enable when a upm lua script exists
#/opt/upm/packages/upm-nightly/bin/upm apply upm@latest
