#ifndef QTAXVIEWER_PLANETOSM_H
#define QTAXVIEWER_PLANETOSM_H
#include <QAxBindable>
#include <QTranslator>
#include <QMap>
#include <QVariant>
#include <QList>
#include <QMutex>
#include "osm_frame_widget.h"

/*!
 \brief	This class inher from osm_frame_widget and QAxBindable,
	Provides event systems and function call interface
 \class	qtaxviewer_planetosm qtaxviewer_planetosm.h "qtviewer_planetosm/qtaxviewer_planetosm.h"
 \author	goldenhawking	\date	2015-12-11
*/
class qtaxviewer_planetosm :public osm_frame_widget, public QAxBindable
{
	Q_OBJECT
private:
	QString map_to_string(const QMap<QString, QVariant> & m);
	QMap<QString, QVariant> string_to_map(const QString & s);
protected:
	QTranslator qtTranslator;
	QTranslator appTranslator;

	//!event Listeners
	QMutex m_mutex_evts;
	QList<QMap<QString, QVariant> > m_list_events;
	void evt_listener(const QMap<QString, QVariant> e);

public:
	explicit qtaxviewer_planetosm(QWidget *parent = 0);
	~qtaxviewer_planetosm();
	//! slots below is designed for activeX interfaces
public slots:
	QString osm_get_remote_address(QString layerName) const;
	void	osm_set_remote_address (QString layerName, QString addr);
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
	QString osm_layer_call_function(QString layerName, QString args);

protected slots:
	//! internal evts
	void _next_pending_evts();
signals:
	void _evt_next_pending_evts();
	//! messages
	void evt_Message(QString);
};

#endif // QTAXVIEWER_PLANETOSM_H
