#include "geographicsscene.h"
#include "geoitembase.h"
#include <QGraphicsItem>
#include <algorithm>
namespace QTVP_GEOMARKER{
	geoGraphicsScene::geoGraphicsScene(QObject *parent) :
		QGraphicsScene(parent)
	{
		this->setItemIndexMethod(QGraphicsScene::NoIndex);
		currentNewLevel = 0;
	}
	geoGraphicsScene::geoGraphicsScene(const QRectF &sceneRect, QObject *parent )
		:QGraphicsScene(sceneRect,parent)
	{

	}

	geoGraphicsScene::geoGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent )
		:QGraphicsScene(x,y,width,height,parent)
	{

	}
	bool geoGraphicsScene::addItem(geoItemBase * item,int /*reserved*/)
	{
		bool res = false;
		QGraphicsItem * pg = dynamic_cast<QGraphicsItem *>(item);
		if (pg)
		{
			const QString & namec = item->item_name();
			if (m_map_items.contains(namec))
			{
				geoItemBase * oldItem = m_map_items[namec];
				if (oldItem != item)
				{
					this->removeItem(oldItem,0);
					this->m_map_items[namec] = item;
					this->addItem(pg);
				}
			}
			else
			{
				this->addItem(pg);
				this->m_map_items[namec] = item;
			}
			res = true;
		}

		return res;
	}

	void geoGraphicsScene::removeItem(geoItemBase * item, int /*reserved*/)
	{
		QGraphicsItem * pg = dynamic_cast<QGraphicsItem *>(item);
		if (pg)
		{
			QString c_name = item->item_name();
			if (m_map_items.contains(c_name))
			{
				this->removeItem(pg);
				m_map_items.remove(c_name);
				delete item;
			}
		}

	}

	/**
	 * @brief  sequentially call virtual function geoItemBase::adjust_coords for every geoItemBase object.
	 *
	 * Since the  scene coord will be zoomed in / out together with level change, all graphics items' coords should
	 * be recalculated in time. the method adjust_item_coords will do this automatically,
	 * @param newLevel the level to which current map is zoomed.
	 */
	void geoGraphicsScene::adjust_item_coords(int newLevel)
	{
		currentNewLevel = newLevel;
		if (total_items()<1024)
		{
			for (QMap<QString, geoItemBase * >::iterator p = m_map_items.begin();p!=m_map_items.end();++p)
			{
				geoItemBase * base = p.value();
				//when this function is called, base->level() is the old level from which
				//  current map is zoomed.
				base->adjust_coords(newLevel);
				//After adjust_coords above, the item "base" is considered to
				// have a valid coord corresponds to current  newLevel
				base->setLevel(newLevel);
			}
		}
		else
			m_queue_level_change = geo_items();


	}
	bool geoGraphicsScene::deal_level_queue()
	{
		int bneedref = 0;
		for (int i=0;i<1024 && m_queue_level_change.empty()==false;++i)
		{
			++bneedref;
			geoItemBase * base = m_queue_level_change.first();
			m_queue_level_change.pop_front();
			//when this function is called, base->level() is the old level from which
			//  current map is zoomed.
			base->adjust_coords(currentNewLevel);
			//After adjust_coords above, the item "base" is considered to
			// have a valid coord corresponds to current  newLevel
			base->setLevel(currentNewLevel);
		}
		if (m_queue_level_change.empty()==true && bneedref)
			return true;
		return false;
	}

	geoItemBase * geoGraphicsScene::geoitem_by_name(const QString & name)
	{
		geoItemBase * it = 0;
		if (m_map_items.contains(name))
			it = m_map_items[name];
		return it;

	}

	QList<geoItemBase *> geoGraphicsScene::geo_items()
	{
		return m_map_items.values();
	}

}
