@echo off

if "%INIT%"=="" (
	set INIT=true
	set PAUSE_BUILD=true
	call Win_0_CheckEnv.bat || goto end
)

title 2_Build.bat

echo ########################
echo # RUNNING BUILD SCRIPT #
echo ########################

set errorBuild=0

set rootDir=%cd%

call %VS_SETUP_DEV%

echo #####################
echo # Building WinRedis #
echo #####################
cd %rootDir%\Dependencies\WinRedis\msvs
echo Building WinRedis...
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 RedisServer.sln || set errorBuild=1

echo ####################
echo # Building hiredis #
echo ####################
cd %rootDir%\Dependencies\Redis\deps\hiredis
mkdir build
cd build
echo Running CMake...
cmake -Wno-dev .. || set errorBuild=1
echo Building hiredis...
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 hiredis.sln || set errorBuild=1

echo ############################
echo # Building redis-plus-plus #
echo ############################
cd %rootDir%\Dependencies\redis-plus-plus
mkdir build
cd build
echo Running CMake...
set libDir=%rootDir%\Dependencies\Redis\deps\hiredis\build\Release
set cmakeArgs=-DHIREDIS_HEADER=%rootDir%\Dependencies\Redis\deps  -DHIREDIS_LIB=%libDir%\hiredis.lib -DCMAKE_PREFIX_PATH=%rootDir%\Dependencies\Redis\deps\hiredis\build\Release -DREDIS_PLUS_PLUS_CXX_STANDARD=11
cmake %cmakeArgs% .. || set errorBuild=1
echo Building hiredis...
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 redis++.sln || set errorBuild=1

echo ################################
echo # Building StackExchange.Redis #
echo ################################
cd %rootDir%\Dependencies\StackExchange.Redis\
echo Nuget Package Restore...
%MS_BUILD% /m /t:restore /verbosity:quiet /noLogo StackExchange.Redis.sln || set errorBuild=1
echo Build Release...
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 StackExchange.Redis.sln || set errorBuild=1

echo ###########################
echo # Building GenerateCsProj #
echo ###########################
cd %rootDir%\DataModel\GenerateCsProj
echo Building Release...
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 GenerateCsProj.sln || set errorBuild=1

echo #####################
echo # Building protobuf #
echo #####################
cd %rootDir%\Dependencies\protobuf\cmake\
mkdir build
cd build
echo Running CMake...
set cmakeArgs=-G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=install -Dprotobuf_MSVC_STATIC_RUNTIME=OFF
cmake %cmakeArgs% .. || set errorBuild=1
echo Building protobuf...
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 protobuf.sln || set errorBuild=1

echo #####################
echo # Building protobuf-c #
echo #####################
cd %rootDir%\Dependencies\protobuf-c\build-cmake\
mkdir build
cd build
echo Running CMake...
set cmakeArgs=-DProtobuf_LIBRARIES=..\..\..\protobuf\cmake\build\Release -DProtobuf_INCLUDE_DIR=..\..\..\protobuf\src -DProtobuf_PROTOC_LIBRARY=libprotoc.lib -DPROTOBUF_PROTOC_EXECUTABLE=protoc.exe -DCMAKE_BINARY_DIR=..\..\..\protobuf\cmake\build\Release
cmake %cmakeArgs% .. || set errorBuild=1
echo Building protobuf...
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 protobuf-c.sln || set errorBuild=1

echo ################################
echo # Building C# Protobuf Runtime #
echo ################################
cd %rootDir%\Dependencies\protobuf\csharp\src
REM echo Nuget Package Restore...
REM %MS_BUILD% /m /t:restore /verbosity:quiet /noLogo Google.Protobuf.sln || set errorBuild=1
echo Build Release...
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 Google.Protobuf.sln || set errorBuild=1

echo ###############################
echo # Building Protobuf DataModel #
echo ###############################
cd %rootDir%\DataModel
call Win_Build.bat || set errorBuild=1

echo ###########################
echo # Building RedisInterface #
echo ###########################
cd %rootDir%\RedisInterface
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo RedisInterface.sln || set errorBuild=1

echo ################################
echo # Building ExampleMicroservice #
echo ################################
cd %rootDir%\ExampleMicroservice
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo ExampleMicroservice.sln || set errorBuild=1

:end
if "%errorBuild%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )
if "%PAUSE_BUILD%"=="true" ( if "%JENKINS%"=="" ( pause ) )
exit /B %errorBuild%