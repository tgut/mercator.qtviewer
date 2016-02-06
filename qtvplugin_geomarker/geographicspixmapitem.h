#ifndef GEOGRAPHICSPIXMAPITEM_H
#define GEOGRAPHICSPIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include "geoitembase.h"
namespace QTVP_GEOMARKER{
	class geoGraphicsPixmapItem : public QGraphicsPixmapItem , public geoItemBase
	{
	protected:
		qreal m_lat;
		qreal m_lon;
		int m_center_offsetx;
		int m_center_offsety;
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent * event);
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

	public:
		explicit geoGraphicsPixmapItem(QString name,QTVOSM::viewer_interface * pVi,
							   qreal cent_lat = 90,
							   qreal cent_lon = 0,
							   qreal center_offsetx = 0,
							   qreal center_offsety = 0);
	public:
		qreal lat() const {return m_lat;}
		qreal lon() const {return m_lon;}
		int centerx() const {return m_center_offsetx;}
		int centery() const {return m_center_offsety;}
		void setCenterOffset(int center_offsetx,int center_offsety);
		void setGeo(qreal cent_lat,qreal cent_lon);
		void adjust_coords(int ncurrLevel);
		QPointF label_pos();
	};
}
#endif // GEOGRAPHICSELLIPSEITEM_H
