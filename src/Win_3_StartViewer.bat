@echo off

start MonitoringTools\ChronologicalMessageWindow\bin\Release\ChronologicalMessageWindow.exe "localhost" "All Messages" || ( echo Failed to start! & pause )