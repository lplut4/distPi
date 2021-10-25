@echo off

title Win_Build.bat

echo #################################
echo # RUNNING PROTOBUF BUILD SCRIPT #
echo #################################

set error=0

if not exist "out\cpp"    ( mkdir out\cpp    || set error=1 )
if not exist "out\csharp" ( mkdir out\csharp || set error=1 )	
if not exist "out\python" ( mkdir out\python || set error=1 )

cd Protos

set csharpOutDirectory=..\out\csharp

set protoc=..\..\Dependencies\protobuf\cmake\build\Release\protoc.exe
set protocOptions=--cpp_out=..\out\cpp --csharp_out=%csharpOutDirectory% --python_out=..\out\python

set list=TimeSpec.proto
set list=%list% LogMessage.proto

%protoc% %protocOptions% %list% || set error=1

set generateCsProj=..\GenerateCsProj\bin\Release\GenerateCsProj.exe
set preprotoFile=..\..\RedisInterface\DataModel\DataModel.csproj.preProto

%generateCsProj% %preprotoFile% %csharpOutDirectory% %list%

cd ..

if "%error%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )