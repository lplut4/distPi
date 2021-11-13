#!/bin/bash

rootDir=$(pwd)

echo $rootDir

cd ..
mkdir build
cd build

mkdir -p Config
mkdir -p Dependencies/redis/src
mkdir -p ExampleMicroservice/build 

cp $rootDir/Nix_3_*.sh .
cp $rootDir/Config/RedisConfig_Linux.conf Config
cp $rootDir/Dependencies/redis/src/redis-benchmark Dependencies/redis/src
cp $rootDir/Dependencies/redis/src/redis-check-aof Dependencies/redis/src
cp $rootDir/Dependencies/redis/src/redis-check-rdb Dependencies/redis/src
cp $rootDir/Dependencies/redis/src/redis-cli Dependencies/redis/src
cp $rootDir/Dependencies/redis/src/redis-sentinel Dependencies/redis/src
cp $rootDir/Dependencies/redis/src/redis-server Dependencies/redis/src
cp $rootDir/ExampleMicroservice/build/ExampleMicroservice ExampleMicroservice/build


