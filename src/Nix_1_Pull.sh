#!/bin/bash

pathToDir="Dependencies"

if [ ! -d "$pathToDir/Redis" ]; then
	echo Pulling Redis...
	mkdir -p Dependencies/_Downloads
	mkdir -p Dependencies/Redis
	cd Dependencies/_Downloads
	wget https://download.redis.io/releases/redis-6.2.6.tar.gz > /dev/null
	tar xzf redis-6.2.6.tar.gz --directory ../Redis --strip-components=1  > /dev/null
	cd ../..
else
	echo "Redis already exists"
fi
	
redisClient1="ServiceStack.Redis"
if [ ! -d "$pathToDir/$redisClient1" ]; then
	echo Pulling $redisClient1...
	mkdir -p Dependencies
	cd Dependencies
	git clone https://github.com/ServiceStack/ServiceStack.Redis.git > /dev/null
	cd ..
else
	echo "$redisClient1 already exists"
fi

protobuf="Protobuf"
if [ ! -d "$pathToDir/$protobuf" ]; then
	echo Pulling $protobuf...
	mkdir -p Dependencies
	cd Dependencies
	git clone https://github.com/protocolbuffers/protobuf.git  > /dev/null
	cd protobuf
	git submodule update --init --recursive  > /dev/null
    ./autogen.sh > /dev/null
	cd ../..
else
	echo "$protobuf already exists"
fi


rm -rf Dependencies/_Downloads
