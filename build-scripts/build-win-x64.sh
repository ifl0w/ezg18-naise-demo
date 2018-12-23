#!/usr/bin/env bash

BUILD_DIR="release-win-x64"

mkdir $BUILD_DIR
cd $BUILD_DIR

export CXXFLAGS=-m64
export CFLAGS=-m64
export LDFLAGS=-m64

cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-mingw32.cmake -DCMAKE_BUILD_TYPE=Release -DNAISE_ENGINE_BUILD_SAMPLES=ON ..

if [[ $? != 0 ]]; then
    exit 1 # terminate and indicate error
fi

make

if [[ $? != 0 ]]; then
    exit 1 # terminate and indicate error
fi

# try to replace line endings for windows build but fail silently if not possible
# (tiny-)gltf does not like unix2dos :(
find . -type f -not -path "*/models/*" -print0 | xargs -0 unix2dos

cd ..
chmod 777 -R $BUILD_DIR # needed for jenkins
