#!/bin/bash

pathToDir="Dependencies"

if [ ! -d "$pathToDir/Redis" ]; then
	mkdir -p Dependencies/_Downloads
	mkdir -p Dependencies/Redis
	cd Dependencies/_Downloads
	wget https://download.redis.io/releases/redis-6.2.6.tar.gz
	tar xzf redis-6.2.6.tar.gz --directory ../Redis --strip-components=1
	cd ../..
else
	echo "Redis already exists"
fi
	
redisClient1="ServiceStack.Redis"
if [ ! -d "$pathToDir/$redisClient1" ]; then
	mkdir -p Dependencies
	cd Dependencies
	git clone https://github.com/ServiceStack/ServiceStack.Redis.git
	cd ..
else
	echo "$redisClient1 already exists"
fi


rm -rf Dependencies/_Downloads
