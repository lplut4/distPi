@echo off

start ExampleMicroservice\x64\Release\ExampleMicroservice.exe localhost || ( echo Could not find executable! & pause )