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
	mkdir Dependencies\_Downloads || set error=1
	cd Dependencies\_Downloads || set error=1
	curl --output redis.tar.gz --url https://download.redis.io/releases/redis-6.2.6.tar.gz >NUL || set error=1
	tar xzf redis.tar.gz --directory ..\Redis --strip-components=1 >NUL || set error=1
	cd ..\..
) else (
	echo Redis already exists
)

echo ##############################
echo # Pulling ServiceStack.Redis #
echo ##############################	
if not exist "Dependencies\ServiceStack.Redis" (
	cd Dependencies || set error=1
	git clone https://github.com/ServiceStack/ServiceStack.Redis.git >NUL || set error=1
	cd ..
) else (
	echo ServiceStack.Redis already exists
)

echo ####################
echo # Pulling Protobuf #
echo ####################
if not exist "Dependencies\Protobuf" (
	cd Dependencies || set error=1
	git clone https://github.com/protocolbuffers/protobuf.git >NUL || set error=1
	cd ..
) else (
	echo Protobuf already exists
)

echo ##############################
echo # Pulling protoc for Windows #
echo ##############################
if not exist "Dependencies\protocWin" (
	mkdir Dependencies\protocWin || set error=1
	mkdir Dependencies\_Downloads
	cd Dependencies\_Downloads || set error=1
	powershell wget https://github.com/protocolbuffers/protobuf/releases/download/v3.18.1/protoc-3.18.1-win32.zip -OutFile protoc-win32.zip || set error=1
	powershell Expand-Archive protoc-win32.zip -DestinationPath ..\protocWin || set error=1
	cd ..\..
) else (
	echo ProtobufWin already exists
)

if exist Dependencies\_Downloads ( rmdir /S /Q Dependencies\_Downloads )

if "%error%" == "1" ( 
	echo # ERRORS OCCURRED
) else (
	echo # SUCCESS!
)

pause
