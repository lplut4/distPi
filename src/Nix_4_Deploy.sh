#!/bin/bash

set rootDir=$(pwd)

cd ..
mkdir build
cd build

mkdir -p Config
mkdir -p Dependencies/redis/src
mkdir -p ExampleMicroservice/build 

copy $rootDir/Nix_3_*.sh .
copy $rootDir/Config/RedisConfig_Linux.conf Config
copy $rootDir/Dependencies/redis/src/redis-* Dependencies/redis/src
copy $rootDir/ExampleMicroservice/build/ExampleMicroservice ExampleMicroservice/build
