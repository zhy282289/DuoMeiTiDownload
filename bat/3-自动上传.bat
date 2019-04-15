@echo off
@echo start autoupload

set /a n=0
:aaa
if %n% leq 3 (
@echo TouTiaoDownload.exe -a %n%
start ../bin/x64/Debug/TouTiaoDownload.exe -a %n%
set /a n+=1
goto :aaa)

