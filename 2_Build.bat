@echo off

set msbuild="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"

cd Dependencies\ServiceStack.Redis\src

set error=0

echo ###############################
echo # Building ServiceStack.Redis #
echo ###############################
echo Nuget Package Restore...
%msbuild% /m /t:restore /verbosity:quiet /noLogo ServiceStack.Redis.sln || set error=1

echo Build Release...
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo ServiceStack.Redis.sln || set error=1

cd ..\..\..
cd RedisInterface

echo ###########################
echo # Building RedisInterface #
echo ###########################
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo RedisInterface.sln


pause