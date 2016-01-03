#ifndef GEOGRAPHICSELLIPSEITEM_H
#define GEOGRAPHICSELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include "geoitembase.h"
namespace QTVP_GEOMARKER{
	class geoGraphicsEllipseItem : public QGraphicsEllipseItem, public geoItemBase
	{
	protected:
		qreal m_lat;
		qreal m_lon;
		qreal m_width;
		qreal m_height;
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent * event);
	public:
		explicit geoGraphicsEllipseItem(QString name,QTVOSM::viewer_interface * pVi,
							   qreal cent_lat = 90,
							   qreal cent_lon = 0,
							   qreal pxwidth = 8,
							   qreal pxheight = 8);
	public:
		qreal lat() const {return m_lat;}
		qreal lon() const {return m_lon;}
		qreal width() const {return m_width;}
		qreal height() const {return m_height;}
		void setSize(qreal pxwidth,qreal pxheight);
		void setCenter(qreal cent_lat,qreal cent_lon);
		void adjust_coords(int nNewLevel);
		QPointF label_pos();
	};
}
#endif // GEOGRAPHICSELLIPSEITEM_H
