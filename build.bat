REM ==============================================================

REM Copyright 2018 Los Alamos National Laboratory
REM Copyright 2009-2018 The Purdue University and Purdue University
REM                     Research Foundation

REM Licensed under the Apache License, Version 2.0 (the "License");
REM you may not use this file except in compliance with the License.
REM You may obtain a copy of the License at

REM     http://www.apache.org/licenses/LICENSE-2.0

REM Unless required by applicable law or agreed to in writing, software
REM distributed under the License is distributed on an "AS IS" BASIS,
REM WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
REM See the License for the specific language governing permissions and
REM limitations under the License.

REM =============================================================


@echo off
set SRCDIR=src\
set DESTDIR=release\
set LIBFLAGS=/Qmkl
set CC=icl

if "%1"=="" goto compile
if /i "%1"=="clean" goto clean
if /i "%1"=="run_dgemm_example" goto run_dgemm_example
if /i "%1"=="run_dgemm_with_timing" goto run_dgemm_with_timing
if /i "%1"=="run_matrix_multiplication" goto run_matrix_multiplication
if /i "%1"=="run_dgemm_threading_effect_example" goto run_dgemm_threading_effect_example
if /i "%1"=="help" goto helpmsg
goto helpmsg

:compile
mkdir %DESTDIR% 2>nul
echo %CC% %LIBFLAGS% /Fo%DESTDIR% %SRCDIR%dgemm_example.c /Fe%DESTDIR%dgemm_example.exe
%CC% %LIBFLAGS% /Fo%DESTDIR% %SRCDIR%dgemm_example.c /Fe%DESTDIR%dgemm_example.exe
echo %CC% %LIBFLAGS% /Fo%DESTDIR% %SRCDIR%dgemm_with_timing.c /Fe%DESTDIR%dgemm_with_timing.exe
%CC% %LIBFLAGS% /Fo%DESTDIR% %SRCDIR%dgemm_with_timing.c /Fe%DESTDIR%dgemm_with_timing.exe
echo %CC% %LIBFLAGS% /Fo%DESTDIR% %SRCDIR%matrix_multiplication.c /Fe%DESTDIR%matrix_multiplication.exe
%CC% %LIBFLAGS% /Fo%DESTDIR% %SRCDIR%matrix_multiplication.c /Fe%DESTDIR%matrix_multiplication.exe
echo %CC% %LIBFLAGS% /Fo%DESTDIR% %SRCDIR%dgemm_threading_effect_example.c /Fe%DESTDIR%dgemm_threading_effect_example.exe
%CC% %LIBFLAGS% /Fo%DESTDIR% %SRCDIR%dgemm_threading_effect_example.c /Fe%DESTDIR%dgemm_threading_effect_example.exe
goto eof

:run_dgemm_example
%DESTDIR%dgemm_example.exe
goto eof

:run_dgemm_with_timing
%DESTDIR%dgemm_with_timing.exe
goto eof

:run_matrix_multiplication
%DESTDIR%matrix_multiplication.exe
goto eof

:run_dgemm_threading_effect_example
%DESTDIR%dgemm_threading_effect_example.exe
goto eof

:helpmsg
echo "Syntax: build [|run_dgemm_example|run_dgemm_with_timing"
echo "               |run_matrix_multiplication"
echo "               |run_dgemm_threading_effect_example|clean]"
echo "     build - build all examples"
echo "     build run_dgemm_example - run dgemm_example"
echo "     build run_dgemm_with_timing - run dgemm_with_timing"
echo "     build run_matrix_multiplication - run matrix_multiplication"
echo "     build run_dgemm_threading_effect_example - run dgemm_threading_effect_example"
echo "     build clean - clean build directory"
goto eof


:clean
echo removing files...
rmdir /Q /S %DESTDIR% 2>nul

:eof
