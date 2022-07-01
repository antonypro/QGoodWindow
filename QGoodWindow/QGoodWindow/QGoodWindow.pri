#The MIT License (MIT)

#Copyright © 2021-2022 Antonio Dias

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
    $$PWD/src/qgoodwindow.cpp

HEADERS += \
    $$PWD/src/qgoodwindow.h

INCLUDEPATH += $$PWD

win32 {
equals(QT_MAJOR_VERSION, 5){
QT += winextras
}

SOURCES += \
    $$PWD/src/shadow.cpp

HEADERS += \
    $$PWD/src/common.h \
    $$PWD/src/shadow.h

LIBS += -lUser32 -lGdi32

DEFINES += QGOODWINDOW
CONFIG += qgoodwindow
}

unix:!mac:!android {
equals(QT_MAJOR_VERSION, 5){
QT += testlib x11extras
}

equals(QT_MAJOR_VERSION, 6){
QT += gui-private
}

SOURCES += \
    $$PWD/src/shadow.cpp

HEADERS += \
    $$PWD/src/shadow.h

LIBS += -lX11

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0

DEFINES += QGOODWINDOW
CONFIG += qgoodwindow
}

mac {
OBJECTIVE_SOURCES += \
    $$PWD/src/macosnative.mm

SOURCES += \
    $$PWD/src/notification.cpp

HEADERS += \
    $$PWD/src/macosnative.h \
    $$PWD/src/notification.h

LIBS += -framework Foundation -framework Cocoa -framework AppKit

DEFINES += QGOODWINDOW
CONFIG += qgoodwindow
}
