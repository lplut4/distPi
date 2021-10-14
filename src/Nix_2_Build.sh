#!/bin/bash


if [ -d "Dependencies/Redis" ]; then
	cd Dependencies/Redis
	echo Building Redis...
	make
	cd ../..
else
	echo "Redis doesn't exist!"
fi

if [ -d "Dependencies/hiredis" ]; then
	cd Dependencies/hiredis
	echo Building hiredis...
	make
	cd ../..
else
	echo "hiredis doesn't exist!"
fi

if [ -d "Dependencies/redis-plus-plus" ]; then
	echo Building redis-plus-plus...
	cd Dependencies/redis-plus-plus
	mkdir build
	cd build
	cmake -DREDIS_PLUS_PLUS_CXX_STANDARD=11 ..
	make
	cd ../..
else
	echo "redis-plus-plus doesn't exist!"
fi

if [ -d "Dependencies/protobuf/src" ]; then
	cd Dependencies/protobuf
	echo Building Protobuf..
	./configure
    make
    make check
	cd ../..
else
	echo "Redis doesn't exist!"
fi
