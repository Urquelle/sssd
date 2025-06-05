rem * Skript für die Microsoft Entwicklerkonsole, angepasst für Clang *

@echo off

set PN=sss
set PP=C:\Users\serge\OneDrive\Entwicklung\c\%PN%
set E=%PP%\e

set CC=clang.exe

set CCFLAGS=-O0 -fexec-charset=utf-8 -D_CRT_SECURE_NO_WARNINGS -std=c17 -fno-exceptions -fno-rtti -g -Werror -Wall -Wextra -Wno-microsoft-enum-forward-reference -Wno-unused-parameter -Wno-unused-variable -Wno-missing-field-initializers -Wno-constant-logical-operand -Wno-switch -Wno-unused-but-set-variable -Wno-logical-op-parentheses -FC -I%PP%/q -I%PP%/t
set LDFLAGS=

rmdir /Q /S %E%
mkdir %E%
pushd %E%

cls

%CC% %CCFLAGS% %PP%\b\sss.c -o sss.exe %LDFLAGS%
rem %CC% %CCFLAGS% %PP%\t\tests.c -o tests.exe %LDFLAGS%

popd
