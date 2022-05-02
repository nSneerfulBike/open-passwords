#!/bin/bash

cd headers
./android-install-dependencies.sh

cd ../res/locales/src
./build-cli.sh
