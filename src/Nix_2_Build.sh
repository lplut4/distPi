#!/bin/bash

if [ -d "Dependencies/Redis" ]; then
	cd Dependencies/Redis
	make
	cd ../..
else
	echo "Redis doesn't exist!"
fi

if [ -d "Dependencies/protobuf/src" ]; then
	cd Dependencies/protobuf/src
	./configure
    make
    make check
    sudo make install
    sudo ldconfig # refresh shared library cache.
	cd ../../..
else
	echo "Redis doesn't exist!"
fi
