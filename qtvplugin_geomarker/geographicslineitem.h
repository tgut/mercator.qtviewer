#ifndef GEOGRAPHICSLINEITEM_H
#define GEOGRAPHICSLINEITEM_H

#include <QGraphicsLineItem>
#include "geoitembase.h"
namespace QTVP_GEOMARKER{
	class geoGraphicsLineItem : public QGraphicsLineItem, public geoItemBase
	{
	protected:
		qreal m_lat1;
		qreal m_lon1;
		qreal m_lat2;
		qreal m_lon2;
		void unwarrp();
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent * event);
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

	public:
		explicit geoGraphicsLineItem(QString name,QTVOSM::viewer_interface * pVi,
									 qreal lat1 = 90,
									 qreal lon1 = 0,
									 qreal lat2 = 90,
									 qreal lon2 = 0
				);
	public:
		qreal lat1() const {return m_lat1;}
		qreal lon1() const {return m_lon1;}
		qreal lat2() const {return m_lat2;}
		qreal lon2() const {return m_lon2;}
		void setGeo(qreal lat1, qreal lon1, qreal lat2, qreal lon2);
		void adjust_coords(int nNewLevel);
		QPointF label_pos();
	};
}
#endif // GEOGRAPHICSLINEITEM_H
