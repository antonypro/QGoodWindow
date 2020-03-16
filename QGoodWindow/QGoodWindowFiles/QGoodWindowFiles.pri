QT      += core gui widgets

win32:QT += winextras gui-private

CONFIG  += c++11

SOURCES += \
        $$PWD/source/qgoodwindow.cpp

win32 {
SOURCES += \
        $$PWD/source/shadow.cpp \
        $$PWD/source/qmfcapp.cpp \
        $$PWD/source/qwinhost.cpp \
        $$PWD/source/qwinwidget.cpp \
        $$PWD/source/winwidget.cpp
}


HEADERS += \
        $$PWD/include/qgoodwindow.h

win32 {
HEADERS += \
        $$PWD/include/common.h \
        $$PWD/include/qmfcapp.h \
        $$PWD/include/qwinhost.h \
        $$PWD/include/qwinwidget.h \
        $$PWD/include/shadow.h \
        $$PWD/include/winwidget.h
}

INCLUDEPATH += $$PWD

win32:LIBS += -lUser32 -lGdi32
