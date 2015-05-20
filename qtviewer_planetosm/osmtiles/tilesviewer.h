#ifndef TILESVIEWER_H
#define TILESVIEWER_H

#include <QWidget>
#include <QList>
#include <QSet>
#include "cProjectionMercator.h"
#include "viewer_interface.h"
namespace Ui {
	class tilesviewer;
}
class layer_interface;
class tilesviewer : public QWidget , viewer_interface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID OSMViewInterface_iid )
	Q_INTERFACES(viewer_interface)

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
protected:
	//Center Lat,Lon
	double m_dCenterX;   //percentage, -0.5~0.5
	double m_dCenterY;   //percentage, -0.5~0.5
	int m_nLevel;        //0-18
	//layers
	QList < layer_interface * > m_listLayers;
	QSet <  layer_interface * > m_setLayers;
public:
	//Convert LLA and DP Coords
	bool oTVP_LLA2DP(double lat,double lon,qint32 * pX,qint32 *pY);
	bool oTVP_DP2LLA(qint32 X,qint32 Y,double  * plat,double * plon);
	//cood convertion
	bool CV_PercentageToPixel(int nLevel,double px,double py,int * nx,int * ny);
public:
	//return current level
	int level(){return m_nLevel; }
	//center
	double centerX(){return m_dCenterX;}
	double centerY(){return m_dCenterY;}
	//return current center view lat, lon
	void centerLLA(double * lat, double * lon){oTVP_DP2LLA(width()/2,height()/2,lat,lon);}
	//drag image for special pixes
	void DragView(int nOffsetX,int nOffsetY);
public:
	//layer Ctrl
	bool addLayer(layer_interface * );
	//layer Ctrl
	void removeLayer(layer_interface * );
	QVector<layer_interface * > layers();
	QVector<QString> layerNames();
	QVector<bool> layerVisibilities();
	QVector<bool> layerActivities();
	void moveLayerUp(layer_interface *);
	void moveLayerDown(layer_interface *);
	void moveLayerTop(layer_interface *);
	void moveLayerBottom(layer_interface *);

public slots:
	//set current level
	void setLevel(int n);
	//set current br level to n-4
	void setBrLevel(int n);
	//force update
	void UpdateWindow();
	//set center LLA
	void setCenterLLA(double lat, double lon);
	//set center LLA, not emit centerChanged
	void setBrCenterLLA(double lat, double lon);
	virtual QRect windowRect(){return this->rect();}
signals:
	void evt_level_changed(int);
	void evt_center_changed(double dLat, double dLon);
	void evt_mouse_moved(double dLat, double dLon);
};

#endif // TILESVIEWER_H
