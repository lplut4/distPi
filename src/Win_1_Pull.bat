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
	mkdir Dependencies\_Downloads
	cd Dependencies\_Downloads || set error=1
	curl --output redis.tar.gz --url https://download.redis.io/releases/redis-6.2.6.tar.gz || set error=1
	tar xzf redis.tar.gz --directory ..\Redis --strip-components=1 || set error=1
	cd ..\..
) else (
	echo Redis already exists
)

echo #############################
echo # Pulling Redis for Windows #
echo #############################
if not exist "Dependencies\WinRedis" (
	mkdir Dependencies\WinRedis || set error=1
	mkdir Dependencies\_Downloads
	cd Dependencies\_Downloads || set error=1
	powershell wget https://github.com/microsoftarchive/redis/archive/refs/tags/win-3.0.504.tar.gz -OutFile WinRedis.tar.gz
	tar xzf WinRedis.tar.gz --directory ..\WinRedis --strip-components=1 || set error=1
	cd ..\..
) else (
	echo WinRedis already exists
)

echo ###########################
echo # Pulling redis-plus-plus #
echo ###########################
if not exist "Dependencies\redis-plus-plus" (
	cd Dependencies || set error=1
	git clone -b 1.3.2 https://github.com/sewenew/redis-plus-plus.git || set error=1
	cd ..
) else (
	echo redis-plus-plus already exists
)

echo ###############################
echo # Pulling StackExchange.Redis #
echo ###############################	
if not exist "Dependencies\StackExchange.Redis" (
	cd Dependencies || set error=1
	git clone -b 2.2.79 https://github.com/StackExchange/StackExchange.Redis.git || set error=1
	cd ..
) else (
	echo StackExchange.Redis already exists
)

echo ####################
echo # Pulling Protobuf #
echo ####################
if not exist "Dependencies\Protobuf" (
	cd Dependencies || set error=1
	git clone -b v3.18.1 https://github.com/protocolbuffers/protobuf.git || set error=1
	cd protobuf
	git submodule update --init --recursive
	cd ..\..
) else (
	echo Protobuf already exists
)

echo ######################
echo # Pulling Protobuf-c #
echo ######################
if not exist "Dependencies\Protobuf-c" (
	cd Dependencies || set error=1
	git clone -b v1.4.0 https://github.com/protobuf-c/protobuf-c.git || set error=1
) else (
	echo Protobuf-c already exists
)

if exist Dependencies\_Downloads ( rmdir /S /Q Dependencies\_Downloads )

if "%error%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )

pause
