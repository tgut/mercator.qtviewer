#ifndef QTVPLUGIN_GEOMARKER_H
#define QTVPLUGIN_GEOMARKER_H

#include <QWidget>
#include <QTranslator>
#include <QStandardItemModel>
#include "geographicsscene.h"
#include "../qtviewer_planetosm/osmtiles/layer_interface.h"
#include "../qtviewer_planetosm/osmtiles/viewer_interface.h"
#include "geoitembase.h"
namespace Ui {
	class qtvplugin_geomarker;
}
using namespace QTVOSM;
class qtvplugin_geomarker : public QWidget,public layer_interface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID OSMLayerInterface_iid )
	Q_INTERFACES(QTVOSM::layer_interface)
public:
	qtvplugin_geomarker(QWidget *parent = 0);
	~qtvplugin_geomarker();
	QWidget *	get_propWindow() {return this;}
	/*! Get graphicalScence, the scence is a World coord in current level.
	 * in plugin system and draw tasks, user should divide class from  QGraphicsItem
	 * and maintain lat, lon coords to world coords according to current level
	 * (through  CV_LLA2World for convenience).
	*/
	QTVP_GEOMARKER::geoGraphicsScene * scene(){return m_pScene;}
private:
	int m_nInstance;

	QTVP_GEOMARKER::geoGraphicsScene * m_pScene;
	QTranslator pluginTranslator;
	Ui::qtvplugin_geomarker *ui;
	viewer_interface * m_pVi;
	bool m_bVisible;
	QString m_SLLibName;
	QString m_SLLibPath;
private:
	int m_nTimerID_refreshUI;
	bool m_bNeedRefresh;
	QStandardItemModel * m_pLineStyleModel;
	QStandardItemModel * m_pGeoItemModel;
	QStandardItemModel * m_pGeoPropModel;
private:
	void SaveSettingsToIni();
	void loadSettingsFromIni();
	void loadTranslations();
	QString inifile();
private:
	void refreshMarks();
	void refreshItemUI(QString markname);
	void refreshProps(QTVP_GEOMARKER::geoItemBase * itm);
	QColor string2color(const QString & s);
	QString color2string(const QColor & c);

	template <class T>
	void update_point		(const QString & name,double lat, double lon, const QColor & colorEdge, const QColor & colorFill, int width, int height);
	void update_line		(const QString & name,double lat1, double lon1,double lat2, double lon2, const QColor & color, int width);
	void update_region		(const QString & name,const QPolygonF latlons, const QColor & colorEdge, const QColor & colorFill, int width);
protected:
	layer_interface *		load_initial_plugin(QString strSLibPath,viewer_interface  * ptrviewer);
	QWidget *				load_prop_window();
	void					load_retranslate_UI();

	bool		is_visible();
	void		set_visible(bool vb);
	QString		get_name();
	void		set_name(QString vb);

	void		cb_paintEvent( QPainter * pImage );
	void		cb_levelChanged(int);
	bool		cb_mousePressEvent(QMouseEvent *);
	bool		cb_mouseReleaseEvent(QMouseEvent *);
	bool		cb_mouseDoubleClickEvent(QMouseEvent *);
	bool		cb_event(const QMap<QString, QVariant>);

	void		timerEvent(QTimerEvent * e);

protected slots:
	void on_pushButton_update_clicked();
	void on_radioButton_tool_point_toggled(bool);
	void on_radioButton_tool_line_toggled(bool);
	void on_radioButton_tool_polygon_toggled(bool);
	void on_toolButton_selColor_pointEdge_clicked();
	void on_toolButton_selColor_pointcolorFill_clicked();
	void on_toolButton_selColor_regionEdge_clicked();
	void on_toolButton_selColor_regionFill_clicked();
	void on_tableView_marks_doubleClicked(const QModelIndex & index);
	void on_pushButton_prop_update_clicked();
	void on_pushButton_prop_delete_clicked();
	void on_pushButton_del_clicked();
	void on_pushButton_pickToLine1_clicked();
	void on_pushButton_pickToLine2_clicked();
	void on_toolButton_selColor_clicked();
	void on_pushButton_getRegion_clicked();
};

template <class T>
void qtvplugin_geomarker::update_point(const QString & name,double lat, double lon, const QColor & colorEdge, const QColor & colorFill, int width, int height)
{
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
	pitem->setPen(QPen(colorEdge));
	pitem->setBrush(QBrush(colorFill));

	if (false==this->m_pScene->addItem(pitem,0))
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
	}

}

#endif // QTVPLUGIN_GEOMARKER_H


