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

cd ..

echo Running protoc...
cd DataModel
./Nix_Build.sh
cd ..

echo Building ExampleMicroservice...
cd ExampleMicroservice
mkdir build
cd build
cmake ..
make
