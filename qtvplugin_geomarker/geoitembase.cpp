#include "geoitembase.h"
#include "geographicsscene.h"
#include "../qtviewer_planetosm/osmtiles/viewer_interface.h"
#include <QTextEdit>
namespace QTVP_GEOMARKER{
	geoItemBase::geoItemBase(QString name,geo_item_type tp,QTVOSM::viewer_interface * pVi)
		:m_pVi(pVi)
		,m_nCurrentLevel(0)
		,m_type(tp)
		,m_name(name)
		,m_pLabelItem(0)
		,m_bPropVisible(false)
	{
		if (pVi)
			m_nCurrentLevel = pVi->level();
	}

	geoItemBase::~geoItemBase()
	{
		if (m_pLabelItem)
		{
			if (m_pLabelItem->scene()==0)
			{
				delete m_pLabelItem;
				m_pLabelItem = 0;
			}
		}
	}
	void geoItemBase::setLevel(int newLevel)
	{
		if (m_pLabelItem && newLevel != level())
			m_pLabelItem->setPos(label_pos());
		m_nCurrentLevel = newLevel;
	}

	void geoItemBase::setViewInterface(QTVOSM::viewer_interface * pVi)
	{
		m_pVi = pVi;
		m_nCurrentLevel = pVi->level();
	}

	QStringList geoItemBase::prop_names()
	{
		return std::move(m_props.keys());
	}
	int geoItemBase::prop_counts()
	{
		return m_props.size();
	}
	QVariantList geoItemBase::prop_values()
	{
		return std::move(m_props.values());
	}

	const QVariant & geoItemBase::prop_data(const QString & k)
	{
		static QVariant invalid_item;
		auto it = m_props.find(k);
		if (it==m_props.end())
			return invalid_item;
		return it.value();
	}

	void geoItemBase::set_prop_data(const QString &k,const  QVariant & v)
	{
		QGraphicsItem * pc = dynamic_cast<QGraphicsItem *> (this);
		m_props[k] = v;
		if (k=="LABEL" && pc)
		{
			if (!m_pLabelItem)
				m_pLabelItem = new QGraphicsTextItem(v.toString(),pc);
			else if (m_bPropVisible==false)
				m_pLabelItem->setPlainText(v.toString());
			m_pLabelItem->setPos(this->label_pos());
		}
	}
	void geoItemBase::del_prop (const QString & k)
	{
		QGraphicsItem * pc = dynamic_cast<QGraphicsItem *> (this);
		m_props.remove(k);
		if (k=="LABEL" && pc)
		{
			if (m_pLabelItem && m_bPropVisible==false)
			{
				if (pc->scene())
					pc->scene()->removeItem(m_pLabelItem);
				m_pLabelItem->deleteLater();
				m_pLabelItem = 0;
			}
		}

	}
	void geoItemBase::show_props(bool bShow)
	{
		QGraphicsItem * pc = dynamic_cast<QGraphicsItem *> (this);
		if (!pc)
			return;
		if (bShow)
		{
			m_bPropVisible = true;
			QString str;
			QStringList propNames = prop_names();
			QVariantList propValues = prop_values();
			int cs = propNames.size();
			for (int i=0;i<cs ;++i)
			{
				str += QString("%1 : %2\n").arg(propNames.first()).arg(propValues.first().toString());
				propNames.pop_front();
				propValues.pop_front();
			}
			if (!m_pLabelItem)
				m_pLabelItem = new QGraphicsTextItem(str,pc);
			else
				m_pLabelItem->setPlainText(str);
			m_pLabelItem->setPos(this->label_pos());

		}
		else
		{
			m_bPropVisible = false;
			QVariant v = prop_data("LABEL");
			if (v.isValid()==false)
			{
				if (m_pLabelItem)
				{
					if (pc->scene())
						pc->scene()->removeItem(m_pLabelItem);
					m_pLabelItem->deleteLater();
					m_pLabelItem = 0;
				}
			}
			else
			{
				if (!m_pLabelItem)
					m_pLabelItem = new QGraphicsTextItem(v.toString(),pc);
				m_pLabelItem->setPlainText(v.toString());
				m_pLabelItem->setPos(this->label_pos());
			}
		}
	}
	bool geoItemBase::props_visible	()
	{
		return m_bPropVisible;
	}
	void geoItemBase::adjustLabelPos()
	{
		if (m_pLabelItem)
			m_pLabelItem->setPos(this->label_pos());
	}
}
