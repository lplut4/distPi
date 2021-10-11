@echo off 

set error=0

echo ########################
echo # RUNNING CLEAN SCRIPT #
echo ########################

if exist "Dependencies" (
	rmdir /S /Q Dependencies || set error=1
) 
	
git clean -fxd || set error=1


if "%error%" == "1" ( 
	echo # ERRORS OCCURRED
) else (
	echo # SUCCESS!
)

pause
