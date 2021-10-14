#!/bin/bash

if [ ! -d "Dependencies/Redis" ]; then
	echo Pulling Redis...
	mkdir -p Dependencies/_Downloads
	mkdir -p Dependencies/Redis
	cd Dependencies/_Downloads
	wget https://download.redis.io/releases/redis-6.2.6.tar.gz
	tar xzf redis-6.2.6.tar.gz --directory ../Redis --strip-components=1
	cd ../..
else
	echo "Redis already exists"
fi
	
if [ ! -d "Dependencies/hiredis" ]; then
	echo Pulling hiredis...
	mkdir -p Dependencies
	cd Dependencies
	git clone https://github.com/redis/hiredis.git
	cd ..
else
	echo "redis-plus-plus already exists"
fi
	
if [ ! -d "Dependencies/redis-plus-plus" ]; then
	echo Pulling redis-plus-plus...
	mkdir -p Dependencies
	cd Dependencies
	git clone https://github.com/sewenew/redis-plus-plus.git
	cd ..
else
	echo "redis-plus-plus already exists"
fi
	
if [ ! -d "Dependencies/ServiceStack.Redis" ]; then
	echo Pulling ServiceStack.Redis...
	mkdir -p Dependencies
	cd Dependencies
	git clone https://github.com/ServiceStack/ServiceStack.Redis.git
	cd ..
else
	echo "ServiceStack.Redis already exists"
fi

if [ ! -d "Dependencies/protobuf" ]; then
	echo Pulling protobuf...
	mkdir -p Dependencies
	cd Dependencies
	git clone https://github.com/protocolbuffers/protobuf.git
	cd protobuf
	git submodule update --init --recursive
    ./autogen.sh > /dev/null
	cd ../..
else
	echo "protobuf already exists"
fi


rm -rf Dependencies/_Downloads
