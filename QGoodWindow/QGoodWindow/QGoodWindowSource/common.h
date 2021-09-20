/*
The MIT License (MIT)

Copyright © 2018-2021 Antonio Dias

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef COMMON_H
#define COMMON_H

#ifdef _WIN32

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT _WIN32_WINNT_VISTA

#endif

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#ifdef Q_OS_LINUX

#define BORDERWIDTH 10 //PIXELS

#define MOVERESIZE_MOVE 8 //X11 Fixed Value

#endif

#if defined Q_OS_LINUX || defined Q_OS_MAC
//These values are mandatory on Linux and arbitrary on macOS,
//using the same for convenience.

#define NO_WHERE -1
#define TOP_LEFT 0
#define TOP 1
#define TOP_RIGHT 2
#define LEFT 7
#define RIGHT 3
#define BOTTOM_LEFT 6
#define BOTTOM 5
#define BOTTOM_RIGHT 4
#define TITLE_BAR 8

#endif

#ifdef Q_OS_WIN

#include <windows.h>

#define BORDERWIDTH (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER))

#endif

#endif // COMMON_H
