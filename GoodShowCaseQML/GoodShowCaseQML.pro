QT       += core gui widgets qml quickwidgets

win32:QT += winextras

CONFIG   += c++11

TARGET = GoodShowCaseQML
TEMPLATE = app

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

RESOURCES += \
        qml.qrc

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
}

include($$PWD/../QGoodWindowFiles/QGoodWindowFiles.pri)
