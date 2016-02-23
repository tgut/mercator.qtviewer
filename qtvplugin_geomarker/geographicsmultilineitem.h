#ifndef GEOGRAPHICSMULTILINEITEM_H
#define GEOGRAPHICSMULTILINEITEM_H

#include <QGraphicsPathItem>
#include <QPolygonF>
#include "geoitembase.h"
namespace QTVP_GEOMARKER{
	class geoGraphicsMultilineItem : public QGraphicsPathItem, public geoItemBase
	{
	protected:
		QPolygonF m_llap;
		void unwarrp();
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent * event);
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

	public:
		explicit geoGraphicsMultilineItem(QString name,QTVOSM::viewer_interface * pVi,
									 const QPolygonF & lla_polygon
				);
	public:
		QPolygonF llas() const {return m_llap;}
		void setGeo(const QPolygonF & lla_polygon);
		void adjust_coords(int nNewLevel);
		QPointF label_pos();
		QPointF center_pos(){
			qreal x = path().elementAt(0).x;
			qreal y = path().elementAt(0).y;
			return QPointF(x,y);
		}
	};
}
#endif // geoGraphicsMultilineItem_H
