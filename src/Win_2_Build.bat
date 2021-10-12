@echo off

title 2_Build.bat

echo ########################
echo # RUNNING BUILD SCRIPT #
echo ########################

set rootDir=%cd%

set msbuild="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
set error=0

echo ###############################
echo # Building ServiceStack.Redis #
echo ###############################
cd %rootDir%\Dependencies\ServiceStack.Redis\src
echo Nuget Package Restore...
%msbuild% /m /t:restore /verbosity:quiet /noLogo ServiceStack.Redis.sln || set error=1
echo Build Release...
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 ServiceStack.Redis.sln || set error=1

echo ################################
echo # Building C# Protobuf Runtime #
echo ################################
cd %rootDir%\Dependencies\protobuf\csharp\src
echo Nuget Package Restore...
%msbuild% /m /t:restore /verbosity:quiet /noLogo Google.Protobuf.sln || set error=1
echo Build Release...
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo /p:WarningLevel=0 Google.Protobuf.sln || set error=1

echo ###########################
echo # Building RedisInterface #
echo ###########################
cd %rootDir%\RedisInterface
%msbuild% /m /t:build /p:Configuration=Release /verbosity:quiet /noLogo RedisInterface.sln || set error=1

if "%error%" == "1" ( 
	echo # ERRORS OCCURRED
) else (
	echo # SUCCESS!
)

pause