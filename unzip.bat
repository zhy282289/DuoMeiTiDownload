@echo off
cd /d %~dp0

"%cd%\exe\7z\7z.exe" e -aoa -spf -y -o"%cd%\bin\x64\Debug\ffmpeg" "%cd%\exe\ffmpeg.7z"



echo 已经解压完毕

if not defined INSTALL_PACKING (
	pause
)