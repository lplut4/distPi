@echo off

Dependencies\microsoftarchive\redis\msvs\x64\Release\redis-cli.exe -h localhost || ( echo Failed to start! & pause )