@echo off

start ExampleMicroservice\build\Release\ExampleMicroservice.exe localhost || ( echo Failed to start! & pause )