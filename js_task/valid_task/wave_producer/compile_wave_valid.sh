#!/bin/bash

DIR="build"

if [ ! -d "$DIR" ]; then
    mkdir "$DIR"
fi

cd build
cmake ..
cmake --build .
cd ..
