TEMPLATE = subdirs
SUBDIRS += \
	qtviewer_planetosm \
	qtwidget_planetosm \
	qtvplugin_grid \
	qtvplugin_geomarker \
	qtwidget_planetosm_designer
qtwidget_planetosm.file = qtviewer_planetosm/qtwidget_planetosm.pro
win32:{
    SUBDIRS +=\
	qtaxviewer_planetosm\
	test_container
    qtaxviewer_planetosm.file = qtviewer_planetosm/qtaxviewer_planetosm.pro
}
