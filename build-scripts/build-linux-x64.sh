#!/usr/bin/env bash

BUILD_DIR="release-linux-x64"

mkdir $BUILD_DIR
cd $BUILD_DIR

export CXXFLAGS=-m64
export CFLAGS=-m64
export LDFLAGS=-m64

cmake -DCMAKE_BUILD_TYPE=Release -DNAISE_ENGINE_BUILD_SAMPLES=ON ..

if [[ $? != 0 ]]; then
    exit 1 # terminate and indicate error
fi

make

if [[ $? != 0 ]]; then
    exit 1 # terminate and indicate error
fi

cd ..
chmod 777 -R $BUILD_DIR # needed for jenkins
