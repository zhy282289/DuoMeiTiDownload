@echo off
cd /d %~dp0

"%cd%\exe\7z\7z.exe" e -aoa -spf -y -o"%cd%\bin\x64\Debug\ffmpeg" "%cd%\exe\ffmpeg.7z"



echo �Ѿ���ѹ���

if not defined INSTALL_PACKING (
	pause
)