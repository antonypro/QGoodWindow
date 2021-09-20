#The MIT License (MIT)

#Copyright © 2021 Antonio Dias

#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:

#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

QT += core gui widgets

CONFIG += c++11

SOURCES += \
    $$PWD/QGoodWindowSource/qgoodwindow.cpp

HEADERS += \
    $$PWD/QGoodWindowSource/qgoodwindow.h

INCLUDEPATH += $$PWD

win32 {
QT += winextras

SOURCES += \
    $$PWD/QGoodWindowSource/shadow.cpp \
    $$PWD/QGoodWindowSource/winwidget.cpp

HEADERS += \
    $$PWD/QGoodWindowSource/common.h \
    $$PWD/QGoodWindowSource/shadow.h \
    $$PWD/QGoodWindowSource/winwidget.h

LIBS += -lUser32 -lGdi32

DEFINES += QGOODWINDOW
CONFIG += qgoodwindow
}

unix:!mac:!android {
QT += x11extras testlib

SOURCES += \
    $$PWD/QGoodWindowSource/shadow.cpp

HEADERS += \
    $$PWD/QGoodWindowSource/shadow.h

LIBS += -lX11

DEFINES += QGOODWINDOW
CONFIG += qgoodwindow
}

mac {
OBJECTIVE_SOURCES += \
    $$PWD/QGoodWindowSource/macosnative.mm

SOURCES += \
    $$PWD/QGoodWindowSource/notification.cpp

HEADERS += \
    $$PWD/QGoodWindowSource/macosnative.h \
    $$PWD/QGoodWindowSource/notification.h

LIBS += -framework Foundation -framework Cocoa -framework AppKit

DEFINES += QGOODWINDOW
CONFIG += qgoodwindow
}
