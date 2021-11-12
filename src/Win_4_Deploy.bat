@echo off

setlocal enableextensions

set rootDir=%cd%

cd ..
mkdir build
cd build

mkdir Config
mkdir Dependencies\microsoftarchive\redis\msvs\x64\Release\
mkdir ExampleMicroservice\build\Release\
mkdir MonitoringTools\ChronologicalMessageWindow\bin\Release\
mkdir MonitoringTools\CommandLineLogViewer\bin\Release\

copy %rootDir%\Win_3_*.bat .
copy %rootDir%\Config\RedisConfig_Windows.conf Config
copy %rootDir%\Dependencies\microsoftarchive\redis\msvs\x64\Release\*.exe Dependencies\microsoftarchive\redis\msvs\x64\Release
copy %rootDir%\MonitoringTools\CommandLineLogViewer\bin\Release\*.dll MonitoringTools\CommandLineLogViewer\bin\Release
copy %rootDir%\MonitoringTools\CommandLineLogViewer\bin\Release\*.exe MonitoringTools\CommandLineLogViewer\bin\Release
copy %rootDir%\MonitoringTools\ChronologicalMessageWindow\bin\Release\*.dll MonitoringTools\ChronologicalMessageWindow\bin\Release
copy %rootDir%\MonitoringTools\ChronologicalMessageWindow\bin\Release\*.exe MonitoringTools\ChronologicalMessageWindow\bin\Release
copy %rootDir%\ExampleMicroservice\build\Release\*.exe ExampleMicroservice\build\Release
copy %rootDir%\ExampleMicroservice\build\Release\*.dll ExampleMicroservice\build\Release

endlocal

pause