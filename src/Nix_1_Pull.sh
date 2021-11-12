#!/bin/bash

echo "#######################"
echo "# RUNNING PULL SCRIPT #"
echo "#######################"

if [ ! -d "Dependencies" ]; then
    mkdir Dependencies
fi

cd Dependencies

gitConfig="-c advice.detachedHead=false --depth 1 --single-branch"

if [ ! -d "redis" ]; then
    echo "Pulling redis..."
    git clone $gitConfig -b 6.2.6 https://github.com/redis/redis.git
else
    echo "Redis already exists"
fi
        
if [ ! -d "redis-plus-plus" ]; then
    echo "Pulling redis-plus-plus..."
    git clone $gitConfig -b 1.3.2 https://github.com/sewenew/redis-plus-plus.git
else
    echo "redis-plus-plus already exists"
fi

if [ ! -d "protobuf" ]; then
    echo "Pulling Protobuf..."
    git clone $gitConfig -b v3.18.1 https://github.com/protocolbuffers/protobuf.git
    cd protobuf
    git submodule update --init --recursive
    ./autogen.sh > /dev/null
    cd ..
else
    echo "protobuf already exists"
fi

cd ..
