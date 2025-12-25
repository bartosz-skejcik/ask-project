# TO MAKE STATIC BUILD YOU MUST USE QMAKE FROM STATIC QT INSTALLATION

CONFIG += C++17 static release
DEFINES += QT_STATIC_BUILD QT_NO_LIBRARY

TARGET = static_main
DESTDIR = bin
OBJECTS_DIR = build

QT = core gui widgets

SOURCES += main.cpp \
           conversion.cpp \
           utils.cpp \
           gui.cpp

HEADERS += conversion.h \
           utils.h \
           gui.h
