TEMPLATE = subdirs
SUBDIRS += \
	qtwidget_planetosm \
	qtwidget_planetosm_designer \
	qtviewer_planetosm \
	qtvplugin_grid \
	qtvplugin_geomarker \
	test_container

qtwidget_planetosm.file = qtviewer_planetosm/qtwidget_planetosm.pro

win32:{
    SUBDIRS +=\
	qtaxviewer_planetosm
    qtaxviewer_planetosm.file = qtviewer_planetosm/qtaxviewer_planetosm.pro
}
