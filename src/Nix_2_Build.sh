#!/bin/bash


if [ -d "Dependencies/Redis" ]; then
	cd Dependencies/Redis
	echo Building Redis...
	make
	cd ../..
else
	echo "Redis doesn't exist!"
fi

if [ -d "Dependencies/protobuf/src" ]; then
	cd Dependencies/protobuf
	echo Building Protobuf..
	./configure
    make
    make check
    sudo make install
    sudo ldconfig
	cd ../..
else
	echo "Redis doesn't exist!"
fi
