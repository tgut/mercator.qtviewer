#ifndef QTVPLUGIN_GEOMARKER_H
#define QTVPLUGIN_GEOMARKER_H

#include <QWidget>
#include <QTranslator>
#include <QStandardItemModel>
#include <QMap>
#include <QXmlStreamAttributes>
#include "geographicsscene.h"
#include "../qtviewer_planetosm/osmtiles/layer_interface.h"
#include "../qtviewer_planetosm/osmtiles/viewer_interface.h"
#include "geoitembase.h"
namespace Ui {
	class qtvplugin_geomarker;
}
using namespace QTVOSM;
/*!
 \brief qtvplugin_geomarker introduces QGraphicesView system, established a common approach for geo marking.
 GEO marker is a vector symbol displays on the background OSM raster map. there are 3 different mark types supported by this plugin.
 1. Point Marks. Include ellipse and rect style mark. these type of mark stand for a single point on earth, with a specified lat, lon .the with and height
 for rect / ellipse circumrect can be specified by user at runtime, in PIXEL. with and height will stay still during map zoom.

 2. Line Mark.	Line mark is a beeline on map. ATTENTION, in Mercator Projection system, the geo shortest path between 2 points on earth is NOT a beeline, that means
 beeline on a map is just for display performance and accessibility. the real path is a curve , which has 2 point of intersections exactly at start position and end position.

 3. Polygon (Region) Mark. Polygon mark is a region on map. borders of a region is painted with lines, for a same reason above, the geo shortest path between 2 points on earth is NOT a beeline either.

 Marks above shares a same style system provided by Qt painter system. pen, brush , font can be setted for each mark.

 Each mark has a Uinque ID called "name", and a type enum called "type". It can also contain several user-defined properties, with a key-value  style storage.
 Especial, a user-defined property called "LABEL"(Upper case) is different agains others. the value of LABEL will be displayed as text items on map all the time,
 but other properties will only visible when user click the mark.

 When the mark is clicked, or double clicked, a event will be fired, so that all plugins and OCX containers will be noticed that a mark (with ID) is clicked.

 \class qtvplugin_geomarker qtvplugin_geomarker.h "qtvplugin_geomarker/qtvplugin_geomarker.h"
*/
class qtvplugin_geomarker : public QWidget,public layer_interface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID OSMLayerInterface_iid )
	Q_INTERFACES(QTVOSM::layer_interface)
private:
	struct tag_xml_mark{
		QString name;
		int		type;
		QPolygonF geoPoints;
		QMap<QString, QString> styles;
		QMap<QString, QString> props;
	};

public:
	qtvplugin_geomarker(QWidget *parent = 0);
	~qtvplugin_geomarker();
	QWidget *	get_propWindow() {return this;}
	QString		get_name();
	/*! Get graphicalScence, the scence is a World coord in current level.
	 * in plugin system and draw tasks, user should divide class from  QGraphicsItem
	 * and maintain lat, lon coords to world coords according to current level
	 * (through  CV_LLA2World for convenience).
	*/
	QTVP_GEOMARKER::geoGraphicsScene * scene(){return m_pScene;}
private:
	int m_nInstance;

	QTVP_GEOMARKER::geoGraphicsScene * m_pScene;	//! the graphics scene object pointer.
	QTranslator pluginTranslator;
	Ui::qtvplugin_geomarker *ui;
	viewer_interface * m_pVi;						//! viewer_interface interface provides coordinats convertions.
	bool m_bVisible;
	QString m_SLLibName;
	QString m_SLLibPath;
private:
	/*! a timer provides timing ui-refresh , instead of immediately refresh when item has been updated.
	 * This timer is just affects UI widgets, Map will be updated immediately otherwise.
	*/
	int m_nTimerID_refreshUI;
	bool m_bNeedRefresh;
	QStandardItemModel * m_pLineStyleModel;
	QStandardItemModel * m_pFillStyleModel;
	QStandardItemModel * m_pGeoItemModel;
	QStandardItemModel * m_pGeoPropModel;

	//persistent functions
private:
	void saveSettingsToIni();
	void loadSettingsFromIni();
	bool saveToXml(QString xml);
	bool loadFromXml(QString xml);
	bool readMark(QXmlStreamReader & reader, tag_xml_mark & mark,QString & errMsg);
	bool readGeo(QXmlStreamReader & reader, tag_xml_mark & mark,QString & errMsg);
	bool readStyle(QXmlStreamReader & reader, tag_xml_mark & mark,QString & errMsg);
	bool readProps(QXmlStreamReader & reader, tag_xml_mark & mark,QString & errMsg);
	QMap<QString,QString> attribs(QXmlStreamAttributes & ats);
	void loadTranslations();
	QString inifile();

	//UI refreshing functions
private:
	void refreshMarks();
	void refreshItemUI(QString markname);
	void refreshProps(QTVP_GEOMARKER::geoItemBase * itm);
	QColor string2color(const QString & s);
	QString color2string(const QColor & c);
	//Geo mark updating functions
private:
	template <class T>
	QTVP_GEOMARKER::geoItemBase * update_point		(const QString & name,double lat, double lon, int width, int height, QPen pen, QBrush brush);
	QTVP_GEOMARKER::geoItemBase * update_line		(const QString & name,double lat1, double lon1,double lat2, double lon2, QPen pen);
	QTVP_GEOMARKER::geoItemBase * update_region		(const QString & name,const QPolygonF latlons, QPen pen, QBrush brush);
	bool update_mark(tag_xml_mark & mark);

	//overloaded virtual funtions
protected:
	layer_interface *		load_initial_plugin(QString strSLibPath,viewer_interface  * ptrviewer);
	QWidget *				load_prop_window();
	void					load_retranslate_UI();

	bool		is_visible();
	void		set_visible(bool vb);
	void		set_name(QString vb);

	void		cb_paintEvent( QPainter * pImage );
	void		cb_levelChanged(int);
	bool		cb_mousePressEvent(QMouseEvent *);
	bool		cb_mouseDoubleClickEvent(QMouseEvent *);
	bool		cb_event(const QMap<QString, QVariant>);

	void		timerEvent(QTimerEvent * e);
	QMap<QString, QVariant> call_func(const  QMap<QString, QVariant> /*paras*/);

	//ui slots
protected slots:
	void on_pushButton_update_clicked();
	void on_radioButton_tool_point_toggled(bool);
	void on_radioButton_tool_line_toggled(bool);
	void on_radioButton_tool_polygon_toggled(bool);
	void on_toolButton_selColorPen_clicked();
	void on_toolButton_selColorFill_clicked();
	void on_toolButton_selColorText_clicked();
	void on_tableView_marks_doubleClicked(const QModelIndex & index);
	void on_pushButton_prop_update_clicked();
	void on_pushButton_prop_delete_clicked();
	void on_pushButton_del_clicked();
	void on_pushButton_pickToLine1_clicked();
	void on_pushButton_pickToLine2_clicked();
	void on_pushButton_getRegion_clicked();
	void on_pushButton_save_clicked();
	void on_pushButton_load_clicked();
};

template <class T>
QTVP_GEOMARKER::geoItemBase * qtvplugin_geomarker::update_point(const QString & name,double lat, double lon, int width, int height, QPen pen, QBrush brush)
{
	QTVP_GEOMARKER::geoItemBase * res = 0;
	//Get raw Item by name
	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	//Get Props
	QStringList propNames;
	QVariantList propValues;
	if (base)
	{
		propNames = base->prop_names();
		propValues = base->prop_values();
	}
	//type convertion to T
	T * pitem = base?dynamic_cast<T  *>(base):0;
	if (!pitem)
		pitem	= new T(name,
						this->m_pVi,
						lat,lon,
						width,height);
	pitem->setPen(pen);
	pitem->setBrush(brush);

	if (base == pitem)
	{
		pitem->setCenter(lat,lon);
		pitem->setSize(width,height);
		res = pitem;
	}
	else if (false==this->m_pScene->addItem(pitem,0))
	{
		if (base != pitem)
			delete pitem;
	}
	else
	{
		int cs = propNames.size();
		for (int i=0;i<cs && base != pitem;++i)
		{
			pitem->set_prop_data(propNames.first(), propValues.first());
			propNames.pop_front();
			propValues.pop_front();
		}
		res = pitem;
	}

	return res;

}

#endif // QTVPLUGIN_GEOMARKER_H


