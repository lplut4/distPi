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

set protoDir=..\..\Dependencies\protocWin
set includeDir=%protoDir%\include
set protoc=%protoDir%\bin\protoc.exe

set protocOptions=--cpp_out=..\out\cpp --csharp_out=..\out\csharp --python_out=..\out\python

%protoc% %protocOptions% TimeSpec.proto LogMessage.proto || set error=1

cd ..

if "%error%" == "1" ( echo # ERRORS OCCURRED ) else ( echo # SUCCESS! )

pause