@echo off

start ExampleMicroservice\x64\Release\ExampleMicroservice.exe localhost || ( echo Failed to start! & pause )