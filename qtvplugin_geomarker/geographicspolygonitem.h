#ifndef GEOGRAPHICSPOLYGONITEM_H
#define GEOGRAPHICSPOLYGONITEM_H

#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include "geoitembase.h"
namespace QTVP_GEOMARKER{
	class geoGraphicsPolygonItem : public QGraphicsPolygonItem, public geoItemBase
	{
	protected:
		QPolygonF m_llap;
		void unwarrp();
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent * event);
	public:
		explicit geoGraphicsPolygonItem(QString name,QTVOSM::viewer_interface * pVi,
									 const QPolygonF & lla_polygon
				);
	public:
		QPolygonF llas() const {return m_llap;}
		void setGeoPolygon(const QPolygonF & lla_polygon);
		void adjust_coords(int nNewLevel);
		QPointF label_pos();
	};
}
#endif // GEOGRAPHICSPOLYGONITEM_H
