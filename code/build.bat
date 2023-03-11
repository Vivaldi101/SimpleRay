@echo off

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
cl -DSTD_C_ENTRY -MT -nologo -Gm- -GR- -EHa- -O2 -Oi -Zi -FC -W4 -wd4101 -wd4706 -wd4201 -wd4100 -wd4189 -wd4996 -wd4505 ../code/main.cpp /link -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib opengl32.lib /out:ray.exe
set last_error=%ERRORLEVEL%
set start_prg=1

IF NOT %last_error%==0 GOTO :end
IF NOT %start_prg%==1 GOTO :end

call ray.exe
popd

:end