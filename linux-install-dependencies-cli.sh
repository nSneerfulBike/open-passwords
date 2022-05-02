#!/bin/bash

cd headers
./linux-install-dependencies.sh

cd ../res/locales/src
./build-cli.sh
