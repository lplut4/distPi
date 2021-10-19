@echo off

title 2_Build.bat

echo ########################
echo # RUNNING BUILD SCRIPT #
echo ########################

set rootDir=%cd%

set msbuild="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
set error=0

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

echo ####################
echo # Building hiredis #
echo ####################
cd %rootDir%\Dependencies\Redis\deps\hiredis
mkdir build
cd build
echo Running CMake...
cmake -Wno-dev .. || set error=1
echo Building hiredis...
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 hiredis.sln || set error=1

echo ############################
echo # Building redis-plus-plus #
echo ############################
cd %rootDir%\Dependencies\redis-plus-plus
mkdir build
cd build
echo Running CMake...
set libDir=%rootDir%\Dependencies\Redis\deps\hiredis\build\Release
set cmakeArgs=-DHIREDIS_HEADER=%rootDir%\Dependencies\Redis\deps  -DHIREDIS_LIB=%libDir%\hiredis.lib -DCMAKE_PREFIX_PATH=%rootDir%\Dependencies\Redis\deps\hiredis\build\Release -DREDIS_PLUS_PLUS_CXX_STANDARD=11
cmake %cmakeArgs% .. || set error=1
echo Building hiredis...
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 redis++.sln || set error=1

echo ################################
echo # Building StackExchange.Redis #
echo ################################
cd %rootDir%\Dependencies\StackExchange.Redis\
echo Nuget Package Restore...
%msbuild% /m /t:restore /verbosity:quiet /noLogo StackExchange.Redis.sln || set error=1
echo Build Release...
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 StackExchange.Redis.sln || set error=1

echo #####################
echo # Building protobuf #
echo #####################
cd %rootDir%\Dependencies\protobuf\cmake\
mkdir build
cd build
echo Running CMake...
set cmakeArgs=-G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=install -Dprotobuf_MSVC_STATIC_RUNTIME=OFF
cmake %cmakeArgs% .. || set error=1
echo Building protobuf...
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 protobuf.sln || set error=1

echo ################################
echo # Building C# Protobuf Runtime #
echo ################################
cd %rootDir%\Dependencies\protobuf\csharp\src
echo Nuget Package Restore...
%msbuild% /m /t:restore /verbosity:quiet /noLogo Google.Protobuf.sln || set error=1
echo Build Release...
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 Google.Protobuf.sln || set error=1

echo ###############################
echo # Building Protobuf DataModel #
echo ###############################
cd %rootDir%\DataModel
call Win_Build.bat || set error=1

echo ###########################
echo # Building RedisInterface #
echo ###########################
cd %rootDir%\RedisInterface
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo RedisInterface.sln || set error=1

echo ################################
echo # Building ExampleMicroservice #
echo ################################
cd %rootDir%\ExampleMicroservice
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo ExampleMicroservice.sln || set error=1

if "%error%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )

pause