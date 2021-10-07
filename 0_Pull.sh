#!/bin/bash

pathToDir="Dependencies"

if [ ! -d "$pathToDir/Redis" ]; then
	mkdir -p Dependencies/_Downloads
	mkdir -p Dependencies/Redis
	cd Dependencies/_Downloads
	wget https://download.redis.io/releases/redis-6.2.6.tar.gz
	tar xzf redis-6.2.6.tar.gz --directory ../Redis --strip-components=1
	cd ../Redis
	make
else
	echo "Redis already exists"
fi
	

