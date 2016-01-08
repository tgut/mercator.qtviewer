#include "geographicsellipseitem.h"
#include "../qtviewer_planetosm/osmtiles/viewer_interface.h"
#include <assert.h>
#include <QGraphicsSceneMouseEvent>
#include <math.h>
#include "qtvplugin_geomarker.h"
namespace QTVP_GEOMARKER{
	geoGraphicsEllipseItem::geoGraphicsEllipseItem(
			QString name,
			QTVOSM::viewer_interface * pVi,
			qreal cent_lat,
			qreal cent_lon,
			qreal pxwidth,
			qreal pxheight)
		:QGraphicsEllipseItem(0)
		,geoItemBase(name,ITEAMTYPE_ELLIPSE_POINT,pVi)
		,m_lat(cent_lat)
		,m_lon(cent_lon)
		,m_width(pxwidth)
		,m_height(pxheight)

	{
		assert(vi()!=0);
		double px,py;
		vi()->CV_LLA2World(m_lat,m_lon,&px,&py);
		setRect(px - pxwidth/2, py - pxheight/2, pxwidth, pxheight);
	}
	void geoGraphicsEllipseItem::adjust_coords(int nNewLevel)
	{
		if (vi() && nNewLevel != level())
		{
			/** Since the map is zooming from level() to current level,
			 * the map size zoom ratio can be calculated using pow below.
			 * We can get new coord for current zoom level by multiplicative.
			*/
			double ratio = pow(2.0,(nNewLevel - level()));
			QRectF rect = this->rect();
			QPointF center = rect.center();
			setRect(center.x() * ratio - rect.width()/2,
					center.y() * ratio - rect.height()/2,
					rect.width(),
					rect.height());
		}
	}

	void geoGraphicsEllipseItem::setSize(qreal pxwidth,qreal pxheight)
	{
		double px,py;
		vi()->CV_LLA2World(m_lat,m_lon,&px,&py);
		m_width = pxwidth;
		m_height = pxheight;
		setRect(px - pxwidth/2, py - pxheight/2, pxwidth, pxheight);
		adjustLabelPos();
	}

	void geoGraphicsEllipseItem::setCenter(qreal cent_lat,qreal cent_lon)
	{
		m_lat = cent_lat;
		m_lon = cent_lon;
		double px,py;
		vi()->CV_LLA2World(cent_lat,cent_lon,&px,&py);
		setRect(px - m_width/2, py - m_height/2, m_width, m_height);
		adjustLabelPos();
	}
	void geoGraphicsEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
	{
		QGraphicsEllipseItem::mousePressEvent(event);
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

	void geoGraphicsEllipseItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
	{
		QGraphicsEllipseItem::mouseDoubleClickEvent(event);
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


	QPointF geoGraphicsEllipseItem::label_pos()
	{
		return QPointF(this->rect().right(),rect().top()-8);
	}
}
