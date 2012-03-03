#-------------------------------------------------
#
# Project created by QtCreator 2012-02-29T21:43:16
#
#-------------------------------------------------

QT      += core gui

TARGET  = DigiPlot
TEMPLATE    = app


SOURCES +=  main.cpp\
            MainWindow.cpp \
    Settings.cpp \
    Utility.cpp \
    SampleThread.cpp \
    DataPlot.cpp

HEADERS +=  MainWindow.h \
    Settings.h \
    Utility.h \
    SampleThread.h \
    DataPlot.h

FORMS   +=  MainWindow.ui

LIBS    +=  -lkdecore \
            -lqwt






















