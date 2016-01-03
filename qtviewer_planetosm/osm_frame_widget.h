#ifndef OSM_FRAME_WIDGET_H
#define OSM_FRAME_WIDGET_H

#include <QWidget>
#include <QMap>
#include <QStandardItemModel>
#include <QMutex>
#include "osmtiles/layer_interface.h"
#include "osmtiles/tilesviewer.h"
using namespace QTVOSM;

namespace Ui {
	class osm_frame_widget;
}

/*!	osm_frame_widget is the container of tilesviewer
 * This call maintains basice UI compments, such like layer table views, buttons, browser views.
 * It also maintain layer order changes, 2 screen dislay.
 *
 \class	osm_frame_widget osm_frame_widget.h "qtviewer_planetosm/osm_frame_widget.h"
  \author	goldenhawking	\date	2015-12-11
*/
class osm_frame_widget : public QWidget
{
	Q_OBJECT

public:
	explicit osm_frame_widget(QWidget *parent = 0);
	~osm_frame_widget();
public:
	//Get tile viewer
	tilesviewer * viewer();

protected:
	bool eventFilter(QObject *obj, QEvent *event);
protected:
	static QMutex m_mutex_proteced;
	Ui::osm_frame_widget *ui;
	QStandardItemModel *m_pLayerDispMod;
	QMap<layer_interface *, QObject *> m_layerPropPages;
	QMap<QObject *, layer_interface *> m_PropPageslayer;
	void UpdateLayerTable();
	bool AppendLayer(QString SLName,layer_interface * interface);
	void EnumPlugins();
protected slots:
	void delacmd_refresh_layer_view();
	void on_tabWidget_main_tabCloseRequested(int index);
	void on_pushButton_visible_clicked();
	void on_pushButton_hide_clicked();
	void on_pushButton_moveUp_clicked();
	void on_pushButton_moveTop_clicked();
	void on_pushButton_moveDown_clicked();
	void on_pushButton_moveBtm_clicked();
	void on_pushButton_active_clicked();
	void on_pushButton_deactive_clicked();
	void on_pushButton_saveToFile_clicked();
};

#endif // OSM_FRAME_WIDGET_H
