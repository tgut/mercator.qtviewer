#-------------------------------------------------
#
# Project created by QtCreator 2015-01-19T17:46:14
#
#-------------------------------------------------

QT       += core gui network axserver
linux:QMAKE_CXXFLAGS += -std=c++11
win32-g++{
	QMAKE_CXXFLAGS += -std=c++11
	CONFIG += no_lflags_merge
	LIBS += -lQt5AxServer -lQt5AxBase -lole32 -loleaut32 -luser32 -lgdi32 -ladvapi32 -luuid
}
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ../../bin/qtaxviewer_planetosm
TEMPLATE = lib
CONFIG  += dll


SOURCES += qtaxviewer_planetosm.cpp\
    osm_frame_widget.cpp \
    osmtiles/cProjectionMercator.cpp \
    osmtiles/tilesviewer.cpp \
    osmtiles/urlDownloader.cpp \
    osmtiles/layer_tiles.cpp \
    osmtiles/layer_browser.cpp \
    osmtiles/layer_tiles_page.cpp


HEADERS  += qtaxviewer_planetosm.h \
    osm_frame_widget.h \
    osmtiles/cProjectionMercator.h \
    osmtiles/tilesviewer.h \
    osmtiles/urlDownloader.h \
    osmtiles/layer_tiles.h \
    osmtiles/layer_browser.h \
    osmtiles/layer_interface.h \
    osmtiles/viewer_interface.h \
    osmtiles/layer_tiles_page.h

FORMS    += osm_frame_widget.ui \
    osmtiles/layer_tiles_page.ui

DEF_FILE = qtaxviewer_planetosm.def
RC_FILE  = qtaxviewer_planetosm.rc
OTHER_FILES += \
	qtaxviewer_planetosm.def\
	qtaxviewer_planetosm.rc\
	qtaxviewer_planetosm.ico\
	qtviewer_planetosm_zh_CN.ts

RESOURCES += \
    resource/resource.qrc

TRANSLATIONS += qtviewer_planetosm_zh_CN.ts

