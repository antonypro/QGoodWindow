#The MIT License (MIT)

#Copyright © 2018-2024 Antonio Dias (https://github.com/antonypro)

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

win32:equals(QT_MAJOR_VERSION, 5){
QT += winextras
}

CONFIG += c++11

SOURCES += \
    $$PWD/src/qgoodwindow.cpp \
    $$PWD/src/qgoodstateholder.cpp \
    $$PWD/src/lightstyle.cpp \
    $$PWD/src/darkstyle.cpp \
    $$PWD/src/stylecommon.cpp

HEADERS += \
    $$PWD/src/qgoodwindow.h \
    $$PWD/src/qgoodstateholder.h \
    $$PWD/src/intcommon.h \
    $$PWD/src/lightstyle.h \
    $$PWD/src/darkstyle.h \
    $$PWD/src/stylecommon.h

RESOURCES += \
    $$PWD/src/qgoodwindow_style.qrc

INCLUDEPATH += $$PWD #include <QGoodWindow>

equals(QT_MAJOR_VERSION, 5){
DEFINES += QT_VERSION_QT5
}
equals(QT_MAJOR_VERSION, 6) {
DEFINES += QT_VERSION_QT6
}

win32 {
LIBS += -lUser32 -lGdi32
}

!no_qgoodwindow{

win32 { #Windows
SOURCES += \
    $$PWD/src/shadow.cpp \
    $$PWD/src/qgooddialog.cpp

HEADERS += \
    $$PWD/src/common.h \
    $$PWD/src/shadow.h \
    $$PWD/src/qgooddialog.h

DEFINES += QGOODWINDOW
CONFIG += qgoodwindow
} #Windows

unix:!mac:!android { #Linux
QT += testlib

equals(QT_MAJOR_VERSION, 5){
QT += x11extras
}

equals(QT_MAJOR_VERSION, 6){
QT += gui-private
}

SOURCES += \
    $$PWD/src/shadow.cpp \
    $$PWD/src/qgooddialog.cpp

HEADERS += \
    $$PWD/src/shadow.h \
    $$PWD/src/qgooddialog.h

QMAKE_CXXFLAGS += -Wno-deprecated-declarations

LIBS += -lX11

CONFIG += link_pkgconfig

equals(QT_MAJOR_VERSION, 5){
PKGCONFIG += gtk+-2.0
}
equals(QT_MAJOR_VERSION, 6){
PKGCONFIG += gtk+-3.0
}

DEFINES += QGOODWINDOW
CONFIG += qgoodwindow
} #Linux

mac { #macOS
OBJECTIVE_SOURCES += \
    $$PWD/src/macosnative.mm

SOURCES += \
    $$PWD/src/notification.cpp \
    $$PWD/src/qgooddialog.cpp

HEADERS += \
    $$PWD/src/macosnative.h \
    $$PWD/src/notification.h \
    $$PWD/src/qgooddialog.h

LIBS += -framework Foundation -framework Cocoa -framework AppKit

DEFINES += QGOODWINDOW
CONFIG += qgoodwindow
} #macOS

}#!no_qgoodwindow
