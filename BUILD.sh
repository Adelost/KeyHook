#!/usr/bin/env bash
BUILD_DIR=build
mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake ../
cmake --build . --config Release
cd ..
