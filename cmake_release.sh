#!/usr/bin/env bash

# builds the cmake files into the build directory

set -e

if [ -d "./build/release/" ]; then
	echo "Release build found, cleaning up..."
	cd ./build/
	rm -rf -- debug/
	mkdir release
	cd release
	cmake -DCMAKE_BUILD_TYPE=RELEASE ../../
else
	echo "Release build not found, making directory..."
	mkdir -p ./build/release/
	cd ./build/release/
	cmake -DCMAKE_BUILD_TYPE=RELEASE ../../
fi
