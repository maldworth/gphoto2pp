#!/usr/bin/env bash

# builds the cmake files into the build directory

set -e

if [ -d "./build/debug/" ]; then
	echo "Debug build found, cleaning up..."
	cd ./build/
	rm -rf -- debug/
	mkdir debug
	cd debug
	cmake -DCMAKE_BUILD_TYPE=DEBUG ../../
else
	echo "Debug build not found, making directory..."
	mkdir -p ./build/debug/
	cd ./build/debug/
	cmake -DCMAKE_BUILD_TYPE=DEBUG ../../
fi
