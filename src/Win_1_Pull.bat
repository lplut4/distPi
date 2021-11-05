@echo off 

title 1_Pull.bat

echo #######################
echo # RUNNING PULL SCRIPT #
echo #######################

set rootDir=%cd%

set error=0
if not exist "Dependencies" ( mkdir Dependencies || set error=1 )

cd Dependencies
set depDir=%cd%

set gitConfig=-c advice.detachedHead=false --depth 1 --single-branch

if not exist "redis" (
	echo Pulling redis...
	git clone %gitConfig% -b 6.2.6 https://github.com/redis/redis.git
) else (
	echo Redis already exists
)
cd %depDir%

if not exist "microsoftarchive" (
	echo Pulling Redis for Windows...
	mkdir microsoftarchive || set error=1	
	cd microsoftarchive
	git clone %gitConfig% -b win-3.0.504 https://github.com/microsoftarchive/redis.git || set error=1
	cd redis
	git apply ..\..\..\Patches\microsoftarchive\redis\0001-Patch-to-allow-builds-in-VS2019.patch
) else (
	echo Redis for Windows already exists
)
cd %depDir%

if not exist "redis-plus-plus" (
	echo Pulling redis-plus-plus...
	git clone %gitConfig% -b 1.3.2 https://github.com/sewenew/redis-plus-plus.git || set error=1
) else (
	echo redis-plus-plus already exists
)
cd %depDir%

if not exist "StackExchange.Redis" (
	echo Pulling StackExchange.Redis...
	git clone %gitConfig% -b 2.2.79 https://github.com/StackExchange/StackExchange.Redis.git || set error=1
	cd StackExchange.Redis
	git apply ..\..\Patches\StackExchange.Redis\0001-Patch-to-remove-github-hooks.patch
) else (
	echo StackExchange.Redis already exists
)
cd %depDir%

if not exist "Newtonsoft.Json" (
	echo Pulling Newtonsoft.Json...
	git clone %gitConfig% -b 13.0.1 https://github.com/JamesNK/Newtonsoft.Json.git || set error=1
	cd Newtonsoft.Json
	git apply ..\..\Patches\Newtonsoft.Json\0001-Patch-to-adjust-the-dotnet-SDK-version.patch
) else (
	echo Newtonsoft.Json already exists
)
cd %depDir%

if not exist "protobuf" (
	echo Pulling protobuf...
	git clone %gitConfig% -b v3.18.1 https://github.com/protocolbuffers/protobuf.git || set error=1
	cd protobuf
	git submodule update --init --recursive
) else (
	echo protobuf already exists
)
cd %depDir%

cd %rootDir%

if "%error%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )

pause
