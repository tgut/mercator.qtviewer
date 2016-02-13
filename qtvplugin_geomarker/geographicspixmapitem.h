#ifndef GEOGRAPHICSPIXMAPITEM_H
#define GEOGRAPHICSPIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include "geoitembase.h"
namespace QTVP_GEOMARKER{
	struct tag_icon{
		QString name;
		QString filename;
		QPixmap icon;
		int centerx;
		int centery;
	};

	class geoGraphicsPixmapItem : public QGraphicsPixmapItem , public geoItemBase
	{
	protected:
		qreal m_lat;
		qreal m_lon;
		const tag_icon * m_pIcon;
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent * event);
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

	public:
		explicit geoGraphicsPixmapItem(QString name,QTVOSM::viewer_interface * pVi
									   ,const tag_icon * pIcon,
									   qreal cent_lat = 90,
									   qreal cent_lon = 0);
	public:
		const tag_icon *  icon(){return m_pIcon;}
		qreal lat() const {return m_lat;}
		qreal lon() const {return m_lon;}
		void setGeo(qreal cent_lat,qreal cent_lon);
		void setPixmap(const tag_icon &icon);
		void adjust_coords(int ncurrLevel);
		QPointF label_pos();
		QPointF center_pos(){return offset();}
	};
}
#endif // GEOGRAPHICSELLIPSEITEM_H
