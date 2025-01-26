@echo off

set PN=sss
set PP=C:\Dev\cpp\%PN%
set E=%PP%\e

set CC=clang++

set ccflags=/Od /utf-8 /D_CRT_SECURE_NO_WARNINGS /std:c++latest -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4624 -wd4530 -wd4244 -wd4201 -wd4100 -wd4101 -wd4189 -wd4505 -wd4127 -wd4702 -wd4310 -FC -Z7 -I%PP%/q
set ldflags= -incremental:no -opt:ref

rmdir /Q /S %E%
mkdir %E%
pushd %E%

cls

cl %ccflags% /DLEX_DEBUG %PP%\b\lex.cpp -Felex.exe /link %ldflags%
cl %ccflags% /DSYN_DEBUG %PP%\b\syn_ausdruck.cpp -Fesyn_ausdruck.exe /link %ldflags%
cl %ccflags% /DSYN_DEBUG %PP%\b\syn_deklaration.cpp -Fesyn_deklaration.exe /link %ldflags%
cl %ccflags% /DSYN_DEBUG %PP%\b\syn_anweisung.cpp -Fesyn_anweisung.exe /link %ldflags%
cl %ccflags% /DSYN_DEBUG %PP%\b\syn_direktive.cpp -Fesyn_direktive.exe /link %ldflags%
cl %ccflags% %PP%\b\sem.cpp -Fesem.exe /link %ldflags%

rem set CCFLAGS=-g -O0 -std=c++20 -I%PP%/q -Werror -Wno-switch
rem set LDFLAGS=
rem 
rem %CC% %CCFLAGS% -DSYN_DEBUG %PP%\b\lex.cpp -o lex.exe %LDFLAGS%
rem %CC% %CCFLAGS% -DSYN_DEBUG %PP%\b\syn_ausdruck.cpp -o syn_ausdruck.exe %LDFLAGS%
rem %CC% %CCFLAGS% -DSYN_DEBUG %PP%\b\syn_deklaration.cpp -o syn_deklaration.exe %LDFLAGS%
rem %CC% %CCFLAGS% -DSYN_DEBUG %PP%\b\syn_anweisung.cpp -o syn_anweisung.exe %LDFLAGS%
rem %CC% %CCFLAGS% -DSYN_DEBUG %PP%\b\syn_direktive.cpp -o syn_direktive.exe %LDFLAGS%
rem %CC% %CCFLAGS%             %PP%\b\sem.cpp           -o sem.exe %LDFLAGS%

popd

