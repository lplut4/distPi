#!/bin/bash

cd Dependencies

echo Building redis...
cd redis
make
cd ..


echo Building hiredis...
cd redis/deps/hiredis
mkdir build
cd build
echo Running cmake...
cmake -Wno-dev ..
echo Running make...
make
cd ../../../..


echo Building redis-plus-plus...
cd redis-plus-plus
mkdir build
cd build
depDir=../..
hiredisDir=$depDir/redis/deps/hiredis
buildDir=$hiredis/build
cmakeOptions="-DREDIS_PLUS_PLUS_BUILD_TEST=OFF -DREDIS_PLUS_PLUS_BUILD_SHARED=OFF -DHIREDIS_HEADER=$depDir/redis/deps -DHIREDIS_LIB=$buildDir/libhiredis_static.a -DTEST_HIREDIS_LIB=$buildDir/hiredis-test -DCMAKE_PREFIX_PATH=$depDir/redis/deps/ -DREDIS_PLUS_PLUS_CXX_STANDARD=11"
cmake $cmakeOptions ..
make
cd ../..


echo Building Protobuf...
cd protobuf
./configure
make
# make check
cd ..


echo Building Protobuf-c...
cd protobuf-c
if [ ! -d "google/protobuf" ]; then
	mkdir google
	cd google
	mkdir protobuf
	cd ..
fi
cp ../protobuf/src/google/protobuf/descriptor.proto google/protobuf/
#cd build-cmake/
#if [ ! -d "build" ]; then 
#	mkdir build
#fi
#cd build
#set cmakeArgs=-DPROTOBUF_PROTOC_EXECUTABLE=../../../protobuf/src/protoc -DCMAKE_BINARY_DIR=../../../protobuf/src/ -DProtobuf_LIBRARIES=../../../protobuf/src/ -DProtobuf_INCLUDE_DIR=../../../protobuf/src/ -DPROTOBUF_LIBRARY=../../../protobuf/src/libprotobuf.la -DPROTOBUF_PROTOC_LIBRARY=../../../protobuf/src/libprotoc.la
#cmake $cmakeArgs ..
./autogen.sh
./configure
make
cd ..

cd ..

echo Building ExampleMicroservice...
cd ExampleMicroservice
mkdir build
cd build
cmake ..
make
