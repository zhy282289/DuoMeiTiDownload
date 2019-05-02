@echo off
@echo start scan

set /a n=1
:aaa
if %n% leq 1 (
@echo TouTiaoDownload.exe -s %n%
start ../bin/x64/Debug/TouTiaoDownload.exe -s -i %n%
set /a n+=1
goto :aaa)

