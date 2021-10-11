@echo off 

if exist "Dependencies" (
	rmdir /S /Q Dependencies
) 
	
git clean -fxd

pause
