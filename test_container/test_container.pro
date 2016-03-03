#-------------------------------------------------
#
# Project created by QtCreator 2015-11-23T17:06:06
#
#-------------------------------------------------

QT       += core gui axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ../../bin/test_container
TEMPLATE = app

LIBS += -L$$OUT_PWD/../bin
LIBS += -lqtwidget_planetosm
INCLUDEPATH += $$PWD/../qtviewer_planetosm

SOURCES += main.cpp\
	testcontainer.cpp

HEADERS  += testcontainer.h

FORMS    += testcontainer.ui
