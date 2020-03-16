QT      += core gui widgets

CONFIG  += c++11

TARGET = ExampleMinimal
TEMPLATE = app

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

include($$PWD/../../QGoodWindowFiles/QGoodWindowFiles.pri)
