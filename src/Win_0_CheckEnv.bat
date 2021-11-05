@echo off

title 0_SetEnv.bat

if "%INIT%"=="" (
	set INIT=true
	set PAUSE_SET_ENV=true
	set PRINT_INFO=true
)

set errorSetEnv=0

set MS_BUILD=""
if not exist %MS_BUILD% ( set MS_BUILD="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" )
if not exist %MS_BUILD% ( set MS_BUILD="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe" )
if not exist %MS_BUILD% ( set MS_BUILD="C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe" )
if not exist %MS_BUILD% ( set MS_BUILD="D:\Program Files (x86)\Microsoft Visual Studio\VS2019\MSBuild\Current\Bin\MSBuild.exe" )
if not exist %MS_BUILD% ( 
	echo Failed to find MSBuild 
	set errorSetEnv=1
)

set VS_SETUP_DEV=""
if not exist %VS_SETUP_DEV% ( set VS_SETUP_DEV="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat" )
if not exist %VS_SETUP_DEV% ( set VS_SETUP_DEV="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat" )
if not exist %VS_SETUP_DEV% ( set VS_SETUP_DEV="C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\Tools\VsDevCmd.bat" )
if not exist %VS_SETUP_DEV% ( set VS_SETUP_DEV="D:\Program Files (x86)\Microsoft Visual Studio\VS2019\Common7\Tools\VsDevCmd.bat" )
if not exist %VS_SETUP_DEV% ( 
	echo Failed to find VsDevCmd.bat 
	set errorSetEnv=1
)

if "%errorSetEnv%"=="1" ( echo ENVIRONMENT NOT SET ) else if "%PRINT_INFO%"=="true" ( echo ENVIRONMENT LOOKS GOOD! )
if "%PAUSE_SET_ENV%"=="true" ( if "%JENKINS%"=="" ( pause ) )
exit /B %errorSetEnv%