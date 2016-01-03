#include "geographicslineitem.h"
#include "../qtviewer_planetosm/osmtiles/viewer_interface.h"
#include <assert.h>
#include <QGraphicsSceneMouseEvent>
#include <math.h>
namespace QTVP_GEOMARKER{
	geoGraphicsLineItem::geoGraphicsLineItem(
			QString name,
			QTVOSM::viewer_interface * pVi,
			qreal lat1 ,
			qreal lon1 ,
			qreal lat2 ,
			qreal lon2)
		:QGraphicsLineItem(0)
		,geoItemBase(name,QTVP_GEOMARKER::ITEAMTYPE_LINE,pVi)
		,m_lat1(lat1)
		,m_lon1(lon1)
		,m_lat2(lat2)
		,m_lon2(lon2)
	{
		assert(vi()!=0);
		double px1,py1,px2,py2;
		unwarrp();
		vi()->CV_LLA2World(m_lat1,m_lon1,&px1,&py1);
		vi()->CV_LLA2World(m_lat2,m_lon2,&px2,&py2);
		setLine(px1,py1,px2,py2);
	}

	void geoGraphicsLineItem::unwarrp()
	{
		while (m_lon2 - m_lon1 < -180)
			m_lon2 += 360;
		while (m_lon2 - m_lon1 > 180)
			m_lon2 -= 360;

		while (m_lon1 < -180)
		{
			m_lon1 += 360;
			m_lon2 += 360;
		}
		while (m_lon1 >180)
		{
			m_lon1 -= 360;
			m_lon2 -= 360;
		}
	}

	void geoGraphicsLineItem::adjust_coords(int nNewLevel)
	{
		if (vi() && nNewLevel != level())
		{
			double ratio = pow(2.0,(nNewLevel - level()));
			QLineF l1 = this->line();
			setLine(l1.x1() * ratio,l1.y1() * ratio,l1.x2() * ratio,l1.y2() * ratio);
		}
	}

	void geoGraphicsLineItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
	{
		QGraphicsLineItem::mousePressEvent(event);
		bool bshow = this->props_visible();
		this->show_props(!bshow);
		//event->accept();
	}
	QPointF geoGraphicsLineItem::label_pos()
	{
		return QPointF(
					(line().x1() + line().x2())/2
					,
					(line().y1() + line().y2())/2 );
	}

	void geoGraphicsLineItem::setGeoLine(qreal lat1, qreal lon1, qreal lat2, qreal lon2)
	{
		m_lat1 = lat1;	m_lon1 = lon1;
		m_lat2 = lat2;	m_lon2 = lon2;
		unwarrp();
		double px1,py1,px2,py2;
		vi()->CV_LLA2World(m_lat1,m_lon1,&px1,&py1);
		vi()->CV_LLA2World(m_lat2,m_lon2,&px2,&py2);
		setLine(px1,py1,px2,py2);
		adjustLabelPos();
	}
}
