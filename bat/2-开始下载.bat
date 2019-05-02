@echo off
@echo start autodownload

set /a n=1
:aaa
if %n% leq 3 (
@echo TouTiaoDownload.exe -d %n%
start ../bin/x64/Debug/TouTiaoDownload.exe -d -i %n%
set /a n+=1
goto :aaa)

