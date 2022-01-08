#!/bin/bash

cd -- "$(dirname "$BASH_SOURCE")"

mkdir -p build/xcode-macos
cd build/xcode-macos

cmake -G Xcode \
-DCMAKE_OSX_DEPLOYMENT_TARGET=10.11 \
-DCMAKE_OSX_ARCHITECTURES=arm64\;x86_64 \
../../..

exit
