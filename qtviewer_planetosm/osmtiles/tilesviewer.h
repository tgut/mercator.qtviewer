#ifndef TILESVIEWER_H
#define TILESVIEWER_H

#include <QWidget>
#include <QList>
#include <QSet>
#include <QMutex>
#include <functional>
#include "cProjectionMercator.h"
#include "viewer_interface.h"
namespace Ui {
	class tilesviewer;
}
namespace QTVOSM{
	class layer_interface;
	/*!	 \brief	tilesviewer is the MAIN_MAP of this application
	 *
	 * It implement all interfaces of viewer_interface, maintain layrs, events, user interaction.
	 * \class	tilesviewer tilesviewer.h "qtviewer_planetosm/osmtiles/tilesviewer.h"
	 *  \author	goldenhawking	\date	2015-12-11
	*/
	class tilesviewer : public QWidget ,public viewer_interface
	{
		Q_OBJECT
		Q_PLUGIN_METADATA(IID OSMViewInterface_iid )
		Q_INTERFACES(QTVOSM::viewer_interface)

	public:
		tilesviewer(QWidget *parent = 0);
		~tilesviewer();

	protected:
		void paintEvent( QPaintEvent * event );
		void mousePressEvent ( QMouseEvent * event );
		void mouseReleaseEvent ( QMouseEvent * event );
		void mouseMoveEvent(QMouseEvent * event);
		void wheelEvent ( QWheelEvent * event )  ;
		void resizeEvent ( QResizeEvent * event ) ;
		void mouseDoubleClickEvent(QMouseEvent * event);
	protected:

		//Center Lat,Lon
		double m_dCenterX;   //percentage, -0.5~0.5
		double m_dCenterY;   //percentage, -0.5~0.5
		int m_nLevel;        //0-18
		//layers
		QList < layer_interface * > m_listLayers;
		QSet <  layer_interface * > m_setLayers;
		//ASyn event systems
		QMutex m_mutex_events;
		QMutex m_mutex_listeners;
		QList < QMap<QString, QVariant> > m_list_pendingEvts ;
		QMap <QString, std::function<void (const QMap<QString, QVariant>)>  > m_map_listeners;
	public:
		//Convert LLA and DP Coords
		bool CV_LLA2DP(double lat,double lon,qint32 * pX,qint32 *pY);
		bool CV_DP2LLA(qint32 X,qint32 Y,double  * plat,double * plon);
		//Convert Merkator and LLA
		bool CV_MK2LLA(double mx, double my, double * plat, double * plon);
		bool CV_LLA2MK(double lat,double lon, double * pmx, double * pmy);
		//Convert Merkator and WorldPixel
		bool CV_MK2World(double mx, double my, double * px, double * py);
		bool CV_World2MK(double x,double y, double * pmx, double * pmy);
		//Convert LLA and World
		bool CV_LLA2World(double lat, double lon, double * px, double * py);
		bool CV_World2LLA(double x,double y, double * plat, double * plon);
		//Convert World and DP
		bool CV_DP2World(qint32 dX, qint32 dY, double * px, double * py);
		bool CV_World2DP(double x,double y, qint32 * dX, qint32 * dY);
		//cood convertion
		bool CV_Pct2World(double px,double py,double * nx,double * ny);
		bool CV_World2Pct(double nx,double ny,double * px,double * py) ;
		//events
		bool send_event(const QMap<QString, QVariant> );
		bool post_event(const QMap<QString, QVariant> );
		void listen_event(QString name,std::function<void (const QMap<QString, QVariant>)> functor);
		void unlisten_event(QString);
	public:
		//return current level
		int level(){return m_nLevel; }
		//center
		double centerX(){return m_dCenterX;}
		double centerY(){return m_dCenterY;}
		//return current center view lat, lon
		void centerLLA(double * lat, double * lon){CV_DP2LLA(width()/2,height()/2,lat,lon);}
		//drag image for special pixes
		void DragView(int nOffsetX,int nOffsetY);
	public:
		//layer Ctrl
		bool addLayer(layer_interface * );
		void removeLayer(layer_interface * );
		bool adjust_layers(layer_interface * target );

		virtual layer_interface * layer(const QString &);
		QVector<layer_interface * > layers();
		QVector<QString> layerNames();
		QVector<bool> layerVisibilities();
		QVector<bool> layerActivities();
		void moveLayerUp(layer_interface *);
		void moveLayerDown(layer_interface *);
		void moveLayerTop(layer_interface *);
		void moveLayerBottom(layer_interface *);
		void updateLayerGridView();
		//force update
		void UpdateWindow();
		//set center LLA
		void setCenterLLA(double lat, double lon);
		//set center LLA, not emit centerChanged
		void setBrCenterLLA(double lat, double lon);
		virtual QRect windowRect(){return this->rect();}
		virtual int windowWidth() {return width();}
		virtual int windowHeight() {return height();}
		bool saveToImage(const QString & strFm);

	public slots:
		//set current level
		void setLevel(int n);
		//set current br level to n-4
		void setBrLevel(int n);
	protected slots:
		void _do_next_evts();
	signals:
		void _inside_do_next_evts();
		void evt_level_changed(int);
		void evt_center_changed(double dLat, double dLon);
		void cmd_update_layer_box();
	};
}
#endif // TILESVIEWER_H
