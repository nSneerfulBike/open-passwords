#!/bin/bash

if [ ! -d "../build" ]; then
    mkdir ../build
fi

g++ -std=c++17 -o ../build/main \
*.cpp \
../headers/cpp-base64/base64.cpp \
-lcryptopp \
-D __android__ \

ret=$?
if [[ ! $ret -eq 0 ]]; then
    echo "Some errors occurred while compiling. Have you run ./install-dependencies.sh ?"
fi
