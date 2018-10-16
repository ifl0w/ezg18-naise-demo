#!/usr/bin/env bash

CONTAINER_TAG="iflow/naise-engine-cross-compile"
WIN_SCRIPT="build-scripts/build-win-x64.sh"
LINUX_SCRIPT="build-scripts/build-linux-x64.sh"

#### Prepare ####

echo -e "\n>>> Pulling newest version of the docker image ... \n"
docker pull $CONTAINER_TAG

if [[ $? != 0 ]]; then
    echo "### Pulling from docker failed."
    exit 1 # terminate and indicate error
fi

#### Windows Build ####

echo -e "\n>>> Building Windows x64 binary ... \n"
docker run -v `pwd`/.:/build -w /build -t $CONTAINER_TAG:latest /build/$WIN_SCRIPT

if [[ $? != 0 ]]; then
    echo "### Building for windows failed."
    exit 1 # terminate and indicate error
fi

#### Linux Build ####

echo -e "\n>>> Building Linux x64 binary ... \n"
docker run -v `pwd`/.:/build -w /build -t $CONTAINER_TAG:latest /build/$LINUX_SCRIPT

if [[ $? != 0 ]]; then
    echo "### Building for linux failed."
    exit 1 # terminate and indicate error
fi