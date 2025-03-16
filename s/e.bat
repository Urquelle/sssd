@echo off

set PN=sss
set PP=C:\Dev\cpp\%PN%
set E=%PP%\e

set CC=cl

set CCFLAGS=/Od /utf-8 /D_CRT_SECURE_NO_WARNINGS /std:c++latest -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4624 -wd4530 -wd4244 -wd4201 -wd4100 -wd4101 -wd4189 -wd4505 -wd4127 -wd4702 -wd4310 -FC -Z7 -I%PP%/q -I%PP%/t
set LDFLAGS= -incremental:no -opt:ref

rmdir /Q /S %E%
mkdir %E%
pushd %E%

cls

%CC% %CCFLAGS% %PP%\b\sss.cpp /Fesss.exe /link %LDFLAGS%
%CC% %CCFLAGS% %PP%\t\tests.cpp /Fetests.exe /link %LDFLAGS%

popd

