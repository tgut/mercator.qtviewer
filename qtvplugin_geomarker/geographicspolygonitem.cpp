#include "geographicspolygonitem.h"
#include "../qtviewer_planetosm/osmtiles/viewer_interface.h"
#include <assert.h>
#include <QGraphicsSceneMouseEvent>
#include <math.h>
#include "geographicsscene.h"
#include "qtvplugin_geomarker.h"
namespace QTVP_GEOMARKER{
	geoGraphicsPolygonItem::geoGraphicsPolygonItem(
			QString name,
			QTVOSM::viewer_interface * pVi,const QPolygonF & lla_polygon)
		:QGraphicsPolygonItem(0)
		,geoItemBase(name,QTVP_GEOMARKER::ITEAMTYPE_REGION,pVi)
	{
		assert(vi()!=0);
		m_llap = lla_polygon;
		unwarrp();
		QPolygonF wp;
		foreach (const QPointF & pt, m_llap)
		{
			double px,py;
			vi()->CV_LLA2World(pt.y(),pt.x(),&px,&py);
			wp<<QPointF(px,py);
		}
		setPolygon(wp);
	}

	void geoGraphicsPolygonItem::unwarrp()
	{
		int sz = m_llap.size();
		if (sz<2)
			return;
		if (m_llap.first() != m_llap.last())
		{
			++sz;
			m_llap.push_back(m_llap.first());
		}
		for (int i=1;i<sz;++i)
		{
			qreal m_lon1 = m_llap[i-1].x();
			qreal m_lon2 = m_llap[i].x();
			while (m_lon2 - m_lon1 < -180)
				m_lon2 += 360;
			while (m_lon2 - m_lon1 > 180)
				m_lon2 -= 360;
			m_llap[i].setX(m_lon2);
		}
	}

	void geoGraphicsPolygonItem::adjust_coords(int nNewLevel)
	{
		if (vi() && nNewLevel != level())
		{
			/** Since the map is zooming from level() to current level,
			 * the map size zoom ratio can be calculated using pow below.
			 * We can get new coord for current zoom level by multiplicative.
			*/
			double ratio = pow(2.0,(nNewLevel - level()));
			QPolygonF p = this->polygon();
			int sz = p.size();
			for (int i=0;i<sz;++i)
			{
				qreal x = p[i].x() * ratio;
				qreal y = p[i].y() * ratio;
				p[i].setX(x);
				p[i].setY(y);
			}
			this->setPolygon(p);
		}
	}

	void geoGraphicsPolygonItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
	{
		QGraphicsPolygonItem::mousePressEvent(event);
		bool bshow = this->props_visible();
		this->show_props(!bshow);
		//post enent
		QMap<QString, QVariant > map_evt;
		geoGraphicsScene * pscene = dynamic_cast<geoGraphicsScene *>(this->scene());
		if (pscene)
		{
			QObject * pPlg = pscene->parent();
			if (pPlg)
			{
				qtvplugin_geomarker * pMarker = dynamic_cast<qtvplugin_geomarker *>(pPlg) ;
				if (pMarker)
				{
					map_evt["source"] = pMarker->get_name();
					map_evt["destin"] = "ALL";
					if (event->buttons() & Qt::LeftButton)
						map_evt["name"] = "ITEM_LBUTTON_CLICKED";
					else if (event->buttons() & Qt::RightButton)
						map_evt["name"] = "ITEM_RBUTTON_CLICKED";
					else if (event->buttons() & Qt::MidButton)
						map_evt["name"] = "ITEM_MBUTTON_CLICKED";
					else
						map_evt["name"] = "ITEM_BUTTON_CLICKED";
					map_evt["id"] = this->item_name();
					vi()->post_event(map_evt);
				}
			}

		}
	}
	void geoGraphicsPolygonItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
	{
		QGraphicsPolygonItem::mouseDoubleClickEvent(event);
		//post enent
		QMap<QString, QVariant > map_evt;
		geoGraphicsScene * pscene = dynamic_cast<geoGraphicsScene *>(this->scene());
		if (pscene)
		{
			QObject * pPlg = pscene->parent();
			if (pPlg)
			{
				qtvplugin_geomarker * pMarker = dynamic_cast<qtvplugin_geomarker *>(pPlg) ;
				if (pMarker)
				{
					map_evt["source"] = pMarker->get_name();
					map_evt["destin"] = "ALL";
					if (event->buttons() & Qt::LeftButton)
						map_evt["name"] = "ITEM_LBUTTON_DBLCLICKED";
					else if (event->buttons() & Qt::RightButton)
						map_evt["name"] = "ITEM_RBUTTON_DBLCLICKED";
					else if (event->buttons() & Qt::MidButton)
						map_evt["name"] = "ITEM_MBUTTON_DBLCLICKED";
					else
						map_evt["name"] = "ITEM_BUTTON_DBLCLICKED";
					map_evt["id"] = this->item_name();
					vi()->post_event(map_evt);
				}
			}

		}
	}

	QPointF geoGraphicsPolygonItem::label_pos()
	{
		QPolygonF p = this->polygon();
		int sz = p.size();
		double x = 0, y = 0;
		for (int i=0;i<sz;++i)
		{
			x += p[i].x();
			y += p[i].y();
		}
		if (sz)
		{
			x /= sz;
			y /= sz;
		}

		return QPointF(x,y);
	}

	void geoGraphicsPolygonItem::setGeoPolygon(const QPolygonF & lla_polygon)
	{
		m_llap = lla_polygon;
		unwarrp();
		QPolygonF wp;
		foreach (const QPointF & pt, m_llap)
		{
			double px,py;
			vi()->CV_LLA2World(pt.y(),pt.x(),&px,&py);
			wp<<QPointF(px,py);
		}
		setPolygon(wp);
		adjustLabelPos();
	}
}
