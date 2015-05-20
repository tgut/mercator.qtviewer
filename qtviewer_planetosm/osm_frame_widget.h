#ifndef OSM_FRAME_WIDGET_H
#define OSM_FRAME_WIDGET_H

#include <QWidget>
#include <QMap>
#include <QStandardItemModel>
#include "osmtiles/layer_interface.h"
namespace Ui {
	class osm_frame_widget;
}

class osm_frame_widget : public QWidget
{
	Q_OBJECT

public:
	explicit osm_frame_widget(QWidget *parent = 0);
	~osm_frame_widget();

private:
	Ui::osm_frame_widget *ui;
	QStandardItemModel *m_pLayerDispMod;
	QMap<layer_interface *, QWidget *> m_layerPropPages;
	void UpdateLayerTable();
	void AppendLayer(layer_interface * interface);
	void EnumPlugins();
public slots:
	void on_pushButton_visible_clicked();
	void on_pushButton_hide_clicked();
	void on_pushButton_moveUp_clicked();
	void on_pushButton_moveTop_clicked();
	void on_pushButton_moveDown_clicked();
	void on_pushButton_moveBtm_clicked();
	void on_pushButton_active_clicked();
};

#endif // OSM_FRAME_WIDGET_H
