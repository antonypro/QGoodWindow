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

CONFIG += c++11

SOURCES += \
    $$PWD/src/qgoodcentralwidget.cpp

HEADERS += \
    $$PWD/src/qgoodcentralwidget.h

INCLUDEPATH += $$PWD #include <QGoodCentralWidget>

DEFINES += QGOODCENTRALWIDGET

qgoodwindow {
QT += svg

SOURCES += \
    $$PWD/src/captionbutton.cpp \
    $$PWD/src/iconwidget.cpp \
    $$PWD/src/titlebar.cpp \
    $$PWD/src/titlewidget.cpp

HEADERS += \
    $$PWD/src/captionbutton.h \
    $$PWD/src/iconwidget.h \
    $$PWD/src/titlebar.h \
    $$PWD/src/titlewidget.h

RESOURCES += \
    $$PWD/src/qgoodcentralwidget_icons.qrc
}
