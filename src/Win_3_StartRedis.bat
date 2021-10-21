@echo off

Dependencies\WinRedis\msvs\x64\Release\redis-server.exe Config\RedisConfig_Windows.conf || ( echo Failed to start! & pause )