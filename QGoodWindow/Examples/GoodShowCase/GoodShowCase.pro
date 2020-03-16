QT       += core gui widgets

win32:QT += winextras

CONFIG   += c++11

TARGET = GoodShowCase
TEMPLATE = app

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

win32 {
SOURCES += \
        captionbutton.cpp \
        iconwidget.cpp \
        titlebar.cpp \
        titlewidget.cpp

HEADERS += \
        captionbutton.h \
        iconwidget.h \
        titlebar.h \
        titlewidget.h

RESOURCES += \
    res.qrc
}

include($$PWD/../../QGoodWindowFiles/QGoodWindowFiles.pri)
