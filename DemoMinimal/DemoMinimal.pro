QT      += core gui widgets

CONFIG  += c++11

TARGET = DemoMinimal
TEMPLATE = app

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

include($$PWD/../QGoodWindowFiles/QGoodWindowFiles.pri)
