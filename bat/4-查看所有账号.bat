@echo off
@echo start autoupload

set /a n=0
:aaa
if %n% leq 3 (
@echo TouTiaoDownload.exe -l %n%
start ../bin/x64/Debug/TouTiaoDownload.exe -l -i %n%
set /a n+=1
goto :aaa)

