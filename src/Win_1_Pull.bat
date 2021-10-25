@echo off 

title 1_Pull.bat

echo #######################
echo # RUNNING PULL SCRIPT #
echo #######################

set error=0

if not exist "Dependencies" ( mkdir Dependencies || set error=1 )

cd Dependencies

echo #################
echo # Pulling Redis #
echo #################
if not exist "redis" (
	git clone -c advice.detachedHead=false -b 6.2.6 https://github.com/redis/redis.git
) else (
	echo Redis already exists
)

echo #############################
echo # Pulling Redis for Windows #
echo #############################
if not exist "microsoftarchive" (
	mkdir microsoftarchive || set error=1	
	cd microsoftarchive
	git clone -c advice.detachedHead=false -b win-3.0.504 https://github.com/microsoftarchive/redis.git || set error=1
	cd ..
) else (
	echo Redis for Windows already exists
)

echo ###########################
echo # Pulling redis-plus-plus #
echo ###########################
if not exist "redis-plus-plus" (
	git clone -c advice.detachedHead=false -b 1.3.2 https://github.com/sewenew/redis-plus-plus.git || set error=1
) else (
	echo redis-plus-plus already exists
)

echo ###############################
echo # Pulling StackExchange.Redis #
echo ###############################	
if not exist "StackExchange.Redis" (
	git clone -c advice.detachedHead=false -b 2.2.79 https://github.com/StackExchange/StackExchange.Redis.git || set error=1
) else (
	echo StackExchange.Redis already exists
)

echo ####################
echo # Pulling protobuf #
echo ####################
if not exist "Protobuf" (
	git clone -c advice.detachedHead=false -b v3.18.1 https://github.com/protocolbuffers/protobuf.git || set error=1
	cd protobuf
	git submodule update --init --recursive
	cd ..
) else (
	echo Protobuf already exists
)

echo ######################
echo # Pulling Protobuf-c #
echo ######################
if not exist "Dependencies\Protobuf-c" (
	git clone -c advice.detachedHead=false -b v1.4.0 https://github.com/protobuf-c/protobuf-c.git || set error=1
) else (
	echo Protobuf-c already exists
)

if "%error%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )

pause
