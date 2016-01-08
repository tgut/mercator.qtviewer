#ifndef GEOGRAPHICSSCENE_H
#define GEOGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QMap>
namespace QTVP_GEOMARKER{
	class geoItemBase;
	/**
	 * @brief geoGraphicsScene shield the common item operations, such as addEllipse
	 * addPolygon, and so on. It provide user several new polymorphism method,
	 * for special geoItemBase classes. Item name will be indexed using a QMap object m_map_items
	 * so that user can get item pointers as soon as possible.
	 *
	 * The scene uses World Pixel Coordinate system, which has a commection between zoom level.
	 * You can learn more principle about coordinates in the comments of class	viewer_interface. in zoom level 0,
	 * world pixel size is 256x256, level 1 is 512x512, level 18 is 67108864 x 67108864
	 * Since the  scene coord will be zoomed in / out together with level change, all graphics items' coords should
	 * be recalculated in time. the method adjust_item_coords will do this automatically,
	 * and in this function, virtual function geoItemBase::adjust_coords will be called sequentially.
	 *
	 */
	class geoGraphicsScene : public QGraphicsScene
	{
		Q_OBJECT
	public:
		geoGraphicsScene(QObject *parent = 0);
		geoGraphicsScene(const QRectF &sceneRect, QObject *parent = 0);
		geoGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0);
	private:
		QMap<QString, geoItemBase * > m_map_items;
		//Overload public functions to provate.
		QGraphicsEllipseItem * addEllipse(const QRectF & rect, const QPen & pen = QPen(), const QBrush & brush = QBrush())
		{return QGraphicsScene::addEllipse(rect,pen,brush);}
		QGraphicsEllipseItem * addEllipse(qreal x, qreal y, qreal w, qreal h, const QPen & pen = QPen(), const QBrush & brush = QBrush())
		{return QGraphicsScene::addEllipse(x,y,w,h,pen,brush);}
		QGraphicsLineItem * addLine(const QLineF & line, const QPen & pen = QPen())
		{return QGraphicsScene::addLine(line,pen);}
		QGraphicsLineItem * addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen & pen = QPen())
		{return QGraphicsScene::addLine(x1,y1,x2,y2,pen);}
		QGraphicsPathItem * addPath(const QPainterPath & path, const QPen & pen = QPen(), const QBrush & brush = QBrush())
		{return QGraphicsScene::addPath(path,pen,brush);}
		QGraphicsPixmapItem * addPixmap(const QPixmap & pixmap)
		{return QGraphicsScene::addPixmap(pixmap);}
		QGraphicsPolygonItem * addPolygon(const QPolygonF & polygon, const QPen & pen = QPen(), const QBrush & brush = QBrush())
		{return QGraphicsScene::addPolygon(polygon,pen,brush);}
		QGraphicsRectItem * addRect(const QRectF & rect, const QPen & pen = QPen(), const QBrush & brush = QBrush())
		{return QGraphicsScene::addRect(rect,pen,brush);}
		QGraphicsRectItem * addRect(qreal x, qreal y, qreal w, qreal h, const QPen & pen = QPen(), const QBrush & brush = QBrush())
		{return QGraphicsScene::addRect(x,y,w,h,pen,brush);}
		QGraphicsSimpleTextItem * addSimpleText(const QString & text, const QFont & font = QFont())
		{return QGraphicsScene::addSimpleText(text,font);}
		QGraphicsTextItem * addText(const QString & text, const QFont & font = QFont())
		{return QGraphicsScene::addText(text,font);}
		QGraphicsProxyWidget * addWidget(QWidget * widget, Qt::WindowFlags wFlags = 0)
		{return QGraphicsScene::addWidget(widget,wFlags);}
		void addItem(QGraphicsItem *item){return QGraphicsScene::addItem(item);}
		void	removeItem(QGraphicsItem * item){return QGraphicsScene::removeItem(item);}
	public :
		//For mutithread opertaions, you should call lock_scene first, and call unlock scene when over
		bool addItem(geoItemBase *item,int /*reserved*/);
		void removeItem(geoItemBase * item, int /*reserved*/);
		geoItemBase * geoitem_by_name(const QString & name);
		void adjust_item_coords(int currentLevel);
		QList<geoItemBase *> geo_items();
	};
}
#endif // GEOGRAPHICSSCENE_H
