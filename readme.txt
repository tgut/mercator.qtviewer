Qt Openstreepmap viewer
---------------------------
We are very glad to publish qplanetosm viewer V1.0.0, a plug-in based cross platform opensource map viewer.

This Project contains a cross-platform qt-based openstreetmap(OSM) viewer.
	* Project qtviewer_planetosm is the main project, provides a plugin system for layer extension.
	* Project qtviewer_planetosm is also a win32 active-X warpper , C#, VB, MFC and Web Apps in win32 will take advantage of this feature.
2 sample plugin examples, demonstrating how plugin works.
	* Project qtvplugin_grid demonstrating low-level painting and event handling using QPainter and Coordinates convertions.
	* Project qtvplugin_geomarker demonstrating heigh-level painting and event handling using QGraphicsScene system. It may be very interesting to see how to deal with map zooming in QGraphicsScene , keeping point marks same pixel size in different level.
1 ActiveX container, demonstrating how to draw animation marks on OSM Map.

