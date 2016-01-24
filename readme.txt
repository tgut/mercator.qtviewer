Qt Mercator Map Viewer (OpenStreetMap based)
---------------------------
We are very glad to publish qplanetosm viewer V1.0.0, a plug-in based cross platform opensource map viewer.
This project was orginally designed for Parkinglots status visualization only. A few month ago,  develop team find that plugin system in qt is strong enough , can do many other things than parkinglots project. Main features :

This Project contains a cross-platform qt-based openstreetmap(OSM) viewer.
	* Project qtviewer_planetosm is the main project, provides a plugin system for layer extension. Layers will be able to call coordinates convertion functions using specail interface provided by main framework; Layers can communicate with main-framework and other plugins using event system and function call system. Both event system and function call system is based on key-value style parament delivering (QMap<QString, QVariant>).
	* Project axqtviewer_planetosm stays at a same phycial path with qtviewer_planetosm, it is a win32 active-X warpper if qtviewer_planetosm. C#, VB, MFC and Old Web Apps in win32 will take advantage of this feature. It also provides a key-value event / function call system , using QString. It is mostly like a bridge across internal QMap<QString, QVariant> calls and external QString calls.
2 sample plugin examples, demonstrating how plugin works.
	* Project qtvplugin_grid demonstrating low-level painting and event handling using QPainter and Coordinates convertions.
	* Project qtvplugin_geomarker demonstrating heigh-level painting and event handling using QGraphicsScene system. It may be very interesting to see how to deal with map zooming in QGraphicsScene , keeping point marks same pixel size in different level.
1 ActiveX container, demonstrating how to draw animation marks on OSM Map.
	* testcontainer is a active-qt based container. in this container, features will be tested such like event system, plugin method system.

There are also several limitations for this project. 
	* The OSM tiles is just a BACKGROUND image, vector data is not supported. For another word, it is impossible to query road names, place names during navigating.
	* Projection system is limited to mercator projection, others is not supported.
	* In Linux system, active-X control is disabled, we will introduce D-bus or TCP server in future instead.
	

