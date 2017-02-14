#-------------------------------------------------
#
# Project created by QtCreator 2017-01-21T09:02:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = 8-puzzle
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    puzzle.cpp \
    algorithmastar.cpp \
    ialgorithm.cpp \
    about.cpp \
    algorithmdls.cpp

HEADERS  += mainwindow.h \
    puzzle.h \
    algorithmastar.h \
    ialgorithm.h \
    about.h \
    algorithmdls.h

FORMS    += mainwindow.ui \
    about.ui

RESOURCES += \
    resources_8puzzle.qrc

RC_ICONS = puzzle.ico

QMAKE_TARGET_CORPORIGHT = ahudz

VERSION = 1.1.0.0
