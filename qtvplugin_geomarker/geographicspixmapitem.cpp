#include "geographicspixmapitem.h"
#include "../qtviewer_planetosm/osmtiles/viewer_interface.h"
#include <assert.h>
#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include "geographicsscene.h"
#include "qtvplugin_geomarker.h"
namespace QTVP_GEOMARKER{
	geoGraphicsPixmapItem::geoGraphicsPixmapItem(
			QString name,
			QTVOSM::viewer_interface * pVi,
			qreal lat,
			qreal lon,
			qreal center_offsetx,
			qreal center_offsety)
		:QGraphicsPixmapItem (0)
		,geoItemBase(name,ITEAMTYPE_PIXMAP,pVi)
		,m_lat(lat)
		,m_lon(lon)
		,m_center_offsetx(center_offsetx)
		,m_center_offsety(center_offsety)

	{
		assert(vi()!=0);
		this->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
		double px,py;
		vi()->CV_LLA2World(m_lat,m_lon,&px,&py);
		setOffset(px - m_center_offsetx, py - m_center_offsety);
	}
	void geoGraphicsPixmapItem::adjust_coords(int ncurrLevel)
	{
		if (vi() && ncurrLevel != level())
		{
			/** Since the map is zooming from level() to current level,
			 * the map size zoom ratio can be calculated using pow below.
			 * We can get new coord for current zoom level by multiplicative.
			*/
			double ratio = pow(2.0,(ncurrLevel - level()));
			QPointF offset = this->offset();
			setOffset(offset.x()*ratio, offset.y()*ratio);
		}
	}

	void geoGraphicsPixmapItem::setCenterOffset(int center_offsetx,int center_offsety)
	{
		double px,py;
		vi()->CV_LLA2World(m_lat,m_lon,&px,&py);
		m_center_offsetx = center_offsetx;
		m_center_offsety = center_offsety;
		setOffset(px - m_center_offsetx, py - m_center_offsety);
	}

	void geoGraphicsPixmapItem::setGeo(qreal cent_lat,qreal cent_lon)
	{
		m_lat = cent_lat;
		m_lon = cent_lon;
		double px,py;
		vi()->CV_LLA2World(cent_lat,cent_lon,&px,&py);
		setOffset(px - m_center_offsetx, py - m_center_offsety);
		adjustLabelPos();
	}
	void geoGraphicsPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
	{
		QGraphicsPixmapItem ::mousePressEvent(event);
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

	void geoGraphicsPixmapItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
	{
		QGraphicsPixmapItem ::mouseDoubleClickEvent(event);
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



	QPointF geoGraphicsPixmapItem::label_pos()
	{
		return QPointF(offset().x()+this->boundingRect().width(),offset().y()-8);
	}
}
