#!/bin/bash

if [ -d "Dependencies/Redis" ]; then
	cd Dependencies/Redis
	make
else
	echo "Redis doesn't exist!"
fi