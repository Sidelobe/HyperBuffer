#!/bin/bash

cd -- "$(dirname "$BASH_SOURCE")"

mkdir -p build/xcode-macos
cd build/xcode-macos

cmake -G Xcode \
-DCMAKE_OSX_DEPLOYMENT_TARGET=10.9 \
../../..

exit
