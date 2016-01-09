#-------------------------------------------------
#
# Project created by QtCreator 2015-01-28T17:23:31
#
#-------------------------------------------------

QT       += widgets
linux:QMAKE_CXXFLAGS += -std=c++11
win32-g++:QMAKE_CXXFLAGS += -std=c++11
TARGET = ../../bin/qtvplugin_geomarker
TEMPLATE = lib

DEFINES += QTVPLUGIN_GRID_LIBRARY

SOURCES += \
    qtvplugin_geomarker.cpp \
    geographicsellipseitem.cpp \
    geoitembase.cpp \
    geographicsscene.cpp \
    geographicsrectitem.cpp \
    qtvplugin_geomarker_uimethods.cpp \
    geographicslineitem.cpp \
    geographicspolygonitem.cpp \
    qtvplugin_geomarker_func.cpp \
    qtvplugin_geomarker_xml.cpp

HEADERS +=\
    ../qtviewer_planetosm/osmtiles/layer_interface.h \
    ../qtviewer_planetosm/osmtiles/viewer_interface.h \
    qtvplugin_geomarker.h \
    geographicsellipseitem.h \
    geoitembase.h \
    geographicsscene.h \
    geographicsrectitem.h \
    geographicslineitem.h \
    geographicspolygonitem.h

FORMS += \
    qtvplugin_geomarker.ui

TRANSLATIONS += qtvplugin_geomarker_zh_CN.ts

OTHER_FILES += qtvplugin_geomarker_zh_CN.ts

RESOURCES += \
    resources.qrc


