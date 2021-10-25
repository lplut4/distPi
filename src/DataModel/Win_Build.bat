@echo off

if "%INIT%"=="" (
	set INIT=true
	set PAUSE_BUILD_PROTOS=true
)

title Win_Build.bat

echo #################################
echo # RUNNING PROTOBUF BUILD SCRIPT #
echo #################################

set errorBuildProtos=0

if not exist "out\c"      ( mkdir out\c      || set errorBuildProtos=1 )
if not exist "out\cpp"    ( mkdir out\cpp    || set errorBuildProtos=1 )
if not exist "out\csharp" ( mkdir out\csharp || set errorBuildProtos=1 )	
if not exist "out\python" ( mkdir out\python || set errorBuildProtos=1 )

cd Protos

set csharpOutDirectory=..\out\csharp

set protoc=..\..\Dependencies\protobuf\cmake\build\Release\protoc.exe
set protocOptions=--cpp_out=..\out\cpp --csharp_out=%csharpOutDirectory% --python_out=..\out\python --c_out=..\out\c

set protoc-gen-c=..\..\Dependencies\protobuf-c\build-cmake\build\Release\protoc-gen-c.exe

set list=TimeSpec.proto
set list=%list% LogMessage.proto

%protoc% %protocOptions% %list% || set errorBuildProtos=1

set generateCsProj=..\GenerateCsProj\bin\Release\GenerateCsProj.exe
set preprotoFile=..\..\RedisInterface\DataModel\DataModel.csproj.preProto

%generateCsProj% %preprotoFile% %csharpOutDirectory% %list%

cd ..

:end
if "%errorBuildProtos%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )
if "%PAUSE_BUILD_PROTOS%"=="true" ( if "%JENKINS%"=="" ( pause ) )
exit /B %errorBuildProtos%