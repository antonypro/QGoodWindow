/*
The MIT License (MIT)

Copyright © 2018-2023 Antonio Dias (https://github.com/antonypro)

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

#include <windows.h>

inline int BORDERWIDTH()
{
    return (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));
}

inline int BORDERHEIGHT()
{
    return (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#endif

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#ifdef Q_OS_LINUX

#define BORDERWIDTH 10 //PIXELS

#define BORDERWIDTHDPI BORDERWIDTH

#define MOVERESIZE_MOVE 8 //X11 Fixed Value

#endif

#if defined Q_OS_LINUX || defined Q_OS_MAC
//The positive values are mandatory on Linux and arbitrary on macOS,
//using the same for convenience.
//The negative values are arbitrary on both platforms.

#define HTNOWHERE -1
#define HTMINBUTTON -2
#define HTMAXBUTTON -3
#define HTCLOSE -4
#define HTTOPLEFT 0
#define HTTOP 1
#define HTTOPRIGHT 2
#define HTLEFT 7
#define HTRIGHT 3
#define HTBOTTOMLEFT 6
#define HTBOTTOM 5
#define HTBOTTOMRIGHT 4
#define HTCAPTION 8

#endif

#endif // COMMON_H
