#ifndef QTWIDGET_PLANETOSM_H
#define QTWIDGET_PLANETOSM_H
#include <QTranslator>
#include <QMap>
#include <QVariant>
#include <QWidget>
#include "osmtiles/viewer_interface.h"

#ifdef PLANETOSM_EXPORT_DLL
#define PLANETOSMDLL Q_DECL_EXPORT
#else
#define PLANETOSMDLL Q_DECL_IMPORT
#endif


/*!
 \brief	This class inher from osm_frame_widget and QtDesigner,
	Provides event systems and function call interface
 \author	goldenhawking	\date	2016-02-14
*/
class PLANETOSMDLL qtwidget_planetosm :public QWidget
{
	Q_OBJECT
protected:
	QWidget * m_map_widget;
	QTranslator qtTranslator;
	QTranslator appTranslator;
	void evt_listener(const QMap<QString, QVariant> e);
	void resizeEvent(QResizeEvent * e);
public:
	explicit qtwidget_planetosm(QWidget *parent = 0);
	~qtwidget_planetosm();
	//! slots below is designed for widget interfaces
public:
	QTVOSM::viewer_interface * viewer();
	QString map_to_string(const QMap<QString, QVariant> & m);
	QMap<QString, QVariant> string_to_map(const QString & s);
	QString osm_get_remote_address(QString layerName) const;
	void	osm_set_remote_address (QString layerName, QString addr);
	QString osm_get_local_cache(QString layerName) const;
	void	osm_set_local_cache (QString layerName, QString addr);
	int		osm_get_cache_expire_days(QString layerName);
	int		osm_set_cache_expire_days(QString layerName,int days);
	void	osm_set_auto_download (QString layerName, int flag);
	int		osm_get_auto_download(QString layerName);
	//Navigate
	int		osm_get_level(void);
	int		osm_set_level(int);
	double	osm_get_center_lat();
	double	osm_get_center_lon();
	int		osm_set_center_pos(double lat,double lon);
	//! \brief	PrintScreen
	int		osm_save_view(QString);
	//layer methods
	int		osm_layer_get_count();
	QString osm_layer_get_name(int n);
	int		osm_layer_set_visiable(QString layerName, int v);
	int		osm_layer_get_visiable(QString layerName);
	int		osm_layer_set_active(QString layerName, int v);
	int		osm_layer_get_active(QString layerName);
	int		osm_layer_move_up(QString layerName);
	int		osm_layer_move_down(QString layerName);
	int		osm_layer_move_top(QString layerName);
	int		osm_layer_move_bottom(QString layerName);
	//function Calls
	QMap<QString, QVariant> osm_layer_call_function(QString layerName, QMap<QString, QVariant> args);
	QString osm_layer_call_function(QString layerName, QString args);
signals:
	void map_event(QMap<QString, QVariant> p);
};

#endif // QTWIDGET_PLANETOSM_H
