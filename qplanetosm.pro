TEMPLATE = subdirs
SUBDIRS += \
	qtviewer_planetosm \
    qtvplugin_grid \
	qtvplugin_geomarker

win32:{
    SUBDIRS +=\
	qtaxviewer_planetosm\
	test_container
    qtaxviewer_planetosm.file = qtviewer_planetosm/qtaxviewer_planetosm.pro
}
