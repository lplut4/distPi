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

echo Building Redis for Windows...
cd %rootDir%\Dependencies\microsoftarchive\redis\msvs
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 RedisServer.sln || set errorBuild=1

echo Building hiredis...
cd %rootDir%\Dependencies\Redis\deps\hiredis
mkdir build
cd build
cmake -Wno-dev .. || set errorBuild=1
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 hiredis.sln || set errorBuild=1

echo Building redis-plus-plus...
cd %rootDir%\Dependencies\redis-plus-plus
mkdir build
cd build
set libDir=%rootDir%\Dependencies\Redis\deps\hiredis\build\Release
set cmakeArgs=-DHIREDIS_HEADER=%rootDir%\Dependencies\Redis\deps  -DHIREDIS_LIB=%libDir%\hiredis.lib -DCMAKE_PREFIX_PATH=%rootDir%\Dependencies\Redis\deps\hiredis\build\Release -DREDIS_PLUS_PLUS_CXX_STANDARD=11
cmake %cmakeArgs% .. || set errorBuild=1
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 redis++.sln || set errorBuild=1

echo Building StackExchange.Redis...
cd %rootDir%\Dependencies\StackExchange.Redis\
%MS_BUILD% /m /t:restore /verbosity:quiet /noLogo StackExchange.Redis.sln || set errorBuild=1
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 StackExchange.Redis.sln || set errorBuild=1

echo Building Newtonsoft.Json...
cd %rootDir%\Dependencies\Newtonsoft.Json\src
%MS_BUILD% /m /t:restore /verbosity:quiet /noLogo Newtonsoft.Json.sln || set errorBuild=1
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 Newtonsoft.Json.sln || set errorBuild=1

echo Building GenerateCsProj...
cd %rootDir%\DataModel\GenerateCsProj
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 GenerateCsProj.sln || set errorBuild=1

echo Building protobuf...
cd %rootDir%\Dependencies\protobuf\cmake\
mkdir build
cd build
set cmakeArgs=-G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=install -Dprotobuf_MSVC_STATIC_RUNTIME=OFF
cmake %cmakeArgs% .. || set errorBuild=1
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 protobuf.sln || set errorBuild=1

echo Building C# Protobuf Runtime...
cd %rootDir%\Dependencies\protobuf\csharp\src
%MS_BUILD% /m /t:restore /verbosity:quiet /noLogo Google.Protobuf.sln || set errorBuild=1
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 Google.Protobuf.sln || set errorBuild=1

echo Building DataModel...
cd %rootDir%\DataModel
call Win_Build.bat || set errorBuild=1

echo Building MonitoringTools...
cd %rootDir%\MonitoringTools
%MS_BUILD% /m /t:restore /verbosity:quiet /noLogo MonitoringTools.sln || set errorBuild=1
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo MonitoringTools.sln || set errorBuild=1

echo Building ExampleMicroservice...
cd %rootDir%\ExampleMicroservice
mkdir build
cd build
cmake ..
%MS_BUILD% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo ExampleMicroservice.sln || set errorBuild=1
copy %rootDir%\Dependencies\redis\deps\hiredis\build\release\hiredis.dll Release

:end
if "%errorBuild%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )
if "%PAUSE_BUILD%"=="true" ( if "%JENKINS%"=="" ( pause ) )
exit /B %errorBuild%