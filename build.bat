@echo off
@echo off
cls
SET msvcdir=C:\Program Files^ (x86)\Microsoft^ Visual^ Studio\2017\Community\VC\Auxiliary\Build\
if not defined DevEnvDir call "%msvcdir%vcvarsall.bat" amd64

set compiler_flags=-I../src/ -nologo /Zi
set executable_name=bevy.exe

IF NOT EXIST build mkdir build
pushd build
cl %compiler_flags% ../src/main.c /link /out:%executable_name% /SUBSYSTEM:console
popd
