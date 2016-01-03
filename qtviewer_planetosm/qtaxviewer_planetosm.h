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
	Q_PROPERTY(QString tileAddress READ tileAddress WRITE setTileAddress)
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
	//! return the tileAddress which OSM plugin take use of
	QString tileAddress(void) const;

	//! slots below is designed for activeX interfaces
public slots:
	void setTileAddress (QString);
	void ConnectToServer (void);
	int IsConnected(void);
	//Navigate
	int GetLevel(void);
	int SetLevel(int);
	double GetCenterLatitude();
	double GetCenterLongitude();
	int SetCenterPos(double lat,double lon);
	//! \brief	PrintScreen
	int SaveCurrentViewToFile(QString);
	//! \brief	acitve/ deactive Map drag
	int FrozenMap(int status);
	//! new interfaces
public slots:

protected slots:
	//! internal evts
	void _next_pending_evts();
signals:
	void _evt_next_pending_evts();
	//! Classical MFC-Styly messages
	void evt_Message(QString ,QString ,double,double,QString);
};

#endif // QTAXVIEWER_PLANETOSM_H
