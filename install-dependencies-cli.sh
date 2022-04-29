#!/bin/bash

cd headers
./install-dependencies.sh

cd ../res/locales/src
./build-cli.sh
