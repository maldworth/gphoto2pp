#!/usr/bin/env bash

# Updates the doxygen documentation, and copies it into the appropriate place
# in the gh-pages branch.

set -e

if [ -d "./build/debug/" ]; then
	echo "Debug build found, making documentation..."
	cd ./build/debug
	make doc
	cd ../../
	tempdir=`mktemp -d`
	cp -r ./doxygen/* ${tempdir}
	if [ -d "./gh-pages/" ]; then
		echo "gh-pages already exists, removing it"
		rm -rf -- gh-pages/
	fi
	mkdir gh-pages
	cd gh-pages
	git init
	git remote add -t gh-pages -f origin https://github.com/maldworth/gphoto2pp.git
	git checkout gh-pages
	cp -rf ${tempdir}/* ./
	rm -rf -- ${tempdir}
	echo "###############"
	echo "Successfully updated doxygen in gh-pages branch. you will need to run 'git status' and see what changes are there"
	echo "depending on the changes, you might have to add or remove files before commiting to staging"
	echo "the easiest command is to use 'git add -A', then you commit changes and push to the remote repo"
else
	echo "Must build in Debug mode in order to generate docs"
fi
