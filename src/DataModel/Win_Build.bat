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

if not exist "out\cpp"    ( mkdir out\cpp    || set errorBuildProtos=1 )
if not exist "out\csharp" ( mkdir out\csharp || set errorBuildProtos=1 )    
if not exist "out\python" ( mkdir out\python || set errorBuildProtos=1 )

cd Protos

set list=Utility\TimeSpec.proto
set list=%list% Utility\LogMessage.proto
set list=%list% Test\Ping.proto
set list=%list% Test\Pong.proto

set csharpOutDirectory=..\out\csharp

set includePath=--proto_path=Utility --proto_path=Test
set protocOptions=%includePath% --cpp_out=..\out\cpp --csharp_out=%csharpOutDirectory% --python_out=..\out\python

set protoc=..\..\Dependencies\protobuf\cmake\build\Release\protoc.exe

%protoc% %protocOptions% %list% || set errorBuildProtos=1

set generateCsProj=..\GenerateCsProj\bin\Release\GenerateCsProj.exe
set preprotoFile=..\..\MonitoringTools\DataModel\DataModel.csproj.preProto

%generateCsProj% %preprotoFile% %csharpOutDirectory% %list%

cd ..

:end
if "%errorBuildProtos%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )
if "%PAUSE_BUILD_PROTOS%"=="true" ( if "%JENKINS%"=="" ( pause ) )
exit /B %errorBuildProtos%