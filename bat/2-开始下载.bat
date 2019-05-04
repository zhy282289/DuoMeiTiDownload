@echo off
@echo start autodownload

set /a n=0
:aaa
if %n% leq 0 (
@echo TouTiaoDownload.exe -d %n%
start ../bin/x64/Debug/TouTiaoDownload.exe -d -i %n%
set /a n+=1
goto :aaa)

