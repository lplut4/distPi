#!/bin/bash

echo "#######################"
echo "# RUNNING PULL SCRIPT #"
echo "#######################"

if [ ! -d "Dependencies" ]; then
	mkdir Dependencies
fi

cd Dependencies

gitConfig="-c advice.detachedHead=false"

if [ ! -d "redis" ]; then
	echo "Pulling redis..."
	git clone $gitConfig -b 6.2.6 https://github.com/redis/redis.git
else
	echo "Redis already exists"
fi
	
if [ ! -d "microsoftarchive" ]; then
	echo "Pulling Redis for Windows..."
	mkdir microsoftarchive
	cd microsoftarchive
	git clone $gitConfig -b win-3.0.504 https://github.com/microsoftarchive/redis.git
	cd redis
	git apply ../../../Patches/microsoftarchive/redis/0001-Patch-to-allow-builds-in-VS2019.patch
	cd ../..
else
	echo "redis-plus-plus already exists"
fi
	
if [ ! -d "redis-plus-plus" ]; then
	echo "Pulling redis-plus-plus..."
	git clone $gitConfig -b 1.3.2 https://github.com/sewenew/redis-plus-plus.git
else
	echo "redis-plus-plus already exists"
fi

if [ ! -d "StackExchange.Redis" ]; then
	echo "Pulling StackExchange.Redis..."
	git clone $gitConfig -b 2.2.79 https://github.com/StackExchange/StackExchange.Redis.git
else
	echo "StackExchange.Redis already exists"
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

if [ ! -d "protobuf-c" ]; then
	echo "Pulling protobuf-c..."
	git clone $gitConfig -b v1.4.0 https://github.com/protobuf-c/protobuf-c.git
else
	echo "protobuf-c already exists"
fi

cd ..
