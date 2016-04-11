#-------------------------------------------------
#
# Project created by QtCreator 2015-11-23T17:06:06
#
#-------------------------------------------------

QT       += core gui
win32: QT+= axcontainer
linux: QMAKE_CXXFLAGS += -std=c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ../../bin/test_container
TEMPLATE = app

win32:LIBS += -L$$OUT_PWD/../bin
linux:LIBS += -L$$OUT_PWD/../../bin

LIBS += -lqtwidget_planetosm
INCLUDEPATH += $$PWD/../qtviewer_planetosm

SOURCES += main.cpp
HEADERS  += testcontainer.h

win32{
 FORMS    += testcontainer.ui
 SOURCES += testcontainer.cpp
}
linux{
 FORMS    += testcontainer_linux.ui
 SOURCES += testcontainer_linux.cpp
}
win32-g++{
	QMAKE_CXXFLAGS += -std=c++11
	CONFIG += no_lflags_merge
	LIBS += -lQt5AxContainer -lQt5AxBase -lole32 -loleaut32 -luser32 -lgdi32 -ladvapi32 -luuid
}
