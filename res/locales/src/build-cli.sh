#!/bin/bash

if [ ! -d "../build" ]; then
    mkdir ../build
fi

echo "Building locales for CLI..."
FILENAME=./build-cli
g++ -o $FILENAME \
$FILENAME.cpp \
../../../headers/cpp-base64/base64.cpp
$FILENAME
rm $FILENAME

echo "Performing final operations..."
FILENAME=./fix-cli
g++ -o $FILENAME \
$FILENAME.cpp \
../../../headers/cpp-base64/base64.cpp
$FILENAME
rm $FILENAME
