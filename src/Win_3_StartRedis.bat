@echo off

start Dependencies\microsoftarchive\redis\msvs\x64\Release\redis-server.exe Config\RedisConfig_Windows.conf || ( echo Failed to start! & pause )