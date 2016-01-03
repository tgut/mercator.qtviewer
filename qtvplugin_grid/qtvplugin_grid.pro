#-------------------------------------------------
#
# Project created by QtCreator 2015-01-28T17:23:31
#
#-------------------------------------------------

QT       += widgets
linux:QMAKE_CXXFLAGS += -std=c++11
win32-g++:QMAKE_CXXFLAGS += -std=c++11
TARGET = ../../bin/qtvplugin_grid
TEMPLATE = lib

DEFINES += QTVPLUGIN_GRID_LIBRARY

SOURCES += \
    qtvplugin_grid.cpp

HEADERS +=\
    ../qtviewer_planetosm/osmtiles/layer_interface.h \
    ../qtviewer_planetosm/osmtiles/viewer_interface.h \
    qtvplugin_grid.h

FORMS += \
    qtvplugin_grid.ui

TRANSLATIONS += qtvplugin_grid_zh_CN.ts

OTHER_FILES += qtvplugin_grid_zh_CN.ts

RESOURCES += \
    resources.qrc


