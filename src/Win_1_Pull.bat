@echo off 

title 1_Pull.bat

echo #######################
echo # RUNNING PULL SCRIPT #
echo #######################

set error=0

if not exist "Dependencies" ( mkdir Dependencies || set error=1 )

echo #################
echo # Pulling Redis #
echo #################
if not exist "Dependencies\Redis" (
	mkdir Dependencies\Redis || set error=1
	mkdir Dependencies\_Downloads || set error=1
	cd Dependencies\_Downloads || set error=1
	curl --output redis.tar.gz --url https://download.redis.io/releases/redis-6.2.6.tar.gz || set error=1
	tar xzf redis.tar.gz --directory ..\Redis --strip-components=1 || set error=1
	cd ..\..
) else (
	echo Redis already exists
)

echo ###########################
echo # Pulling redis-plus-plus #
echo ###########################
if not exist "Dependencies\redis-plus-plus" (
	cd Dependencies || set error=1
	git clone https://github.com/sewenew/redis-plus-plus.git || set error=1
	cd ..
) else (
	echo redis-plus-plus already exists
)

echo ###############################
echo # Pulling StackExchange.Redis #
echo ###############################	
if not exist "Dependencies\StackExchange.Redis" (
	cd Dependencies || set error=1
	git clone https://github.com/StackExchange/StackExchange.Redis.git || set error=1
	cd ..
) else (
	echo StackExchange.Redis already exists
)

echo ####################
echo # Pulling Protobuf #
echo ####################
if not exist "Dependencies\Protobuf" (
	cd Dependencies || set error=1
	git clone https://github.com/protocolbuffers/protobuf.git || set error=1
	cd protobuf
	git submodule update --init --recursive
	cd ..\..
) else (
	echo Protobuf already exists
)

if exist Dependencies\_Downloads ( rmdir /S /Q Dependencies\_Downloads )

if "%error%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )

pause
