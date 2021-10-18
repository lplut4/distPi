#!/bin/bash


if [ -d "Dependencies/Redis" ]; then
	cd Dependencies/Redis
	echo Building Redis...
	make
	cd ../..
else
	echo "Redis doesn't exist!"
fi

if [ -d "Dependencies/Redis/deps/hiredis" ]; then
	cd Dependencies/Redis/deps/hiredis
	echo Building hiredis...
	mkdir build
	cd build
	echo Running cmake...
	cmake ..
	echo Running make...
	make
	cd ../../../../..
else
	echo "hiredis doesn't exist!"
fi

if [ -d "Dependencies/redis-plus-plus" ]; then
	echo Building redis-plus-plus...
	cd Dependencies/redis-plus-plus
	mkdir build
	cd build
	depDir=/home/pi/Dev/distPi/src/Dependencies
	hiredisDir=$depDir/Redis/deps/hiredis
	buildDir=$hiredis/build
	cmakeOptions="-DREDIS_PLUS_PLUS_BUILD_TEST=OFF -DREDIS_PLUS_PLUS_BUILD_SHARED=OFF -DINCLUDEDIR=$depDir -DHIREDIS_HEADER=$depDir -DHIREDIS_LIB=$buildDir/libhiredis_static.a -DTEST_HIREDIS_LIB=$buildDir/hiredis-test -DCMAKE_PREFIX_PATH=$depDir -DREDIS_PLUS_PLUS_CXX_STANDARD=11"
	echo Running cmake...
	cmake $cmakeOptions ..
	echo Running make...
	make
	cd ../../..
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
	echo "protobuf doesn't exist!"
fi
