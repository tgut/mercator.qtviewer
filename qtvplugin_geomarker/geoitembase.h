#ifndef GEOITEMBASE_H
#define GEOITEMBASE_H
#include <QPointF>
#include <QRectF>
#include <QMap>
#include <QVariant>
#include <QFont>
#include <QColor>
#include <QGraphicsTextItem>
namespace QTVOSM{
	class viewer_interface;
}

namespace QTVP_GEOMARKER{
	enum geo_item_type{
		ITEAMTYPE_NULL = 0,
		ITEAMTYPE_ELLIPSE_POINT = 1,
		ITEAMTYPE_RECT_POINT = 2,
		ITEAMTYPE_LINE = 3,
		ITEAMTYPE_REGION = 4
	};
	inline const  QString & item_name_by_enum(geo_item_type tp)
	{
		static QString names[] = {
			"NULL",
			"ELLIPSE_POINT",
			"RECT_POINT",
			"LINE",
			"REGION"
		};
		return names[(int)tp];
	}

	class geoGraphicsScene;
	class geoItemBase
	{
		friend class geoGraphicsScene;
	public:
		geoItemBase(QString name, geo_item_type tp, QTVOSM::viewer_interface * pVi);
		virtual ~geoItemBase();
		QTVOSM::viewer_interface * vi() const {return m_pVi;}
		void setViewInterface(QTVOSM::viewer_interface * pVi);

	private:
		QFont						m_LabelTextFont;
		QColor						m_LabelTextColor;
		bool						m_bPropVisible;
		int							m_nCurrentLevel;
		QGraphicsTextItem *			m_pLabelItem;
		QMap<QString, QVariant>		m_props;
		geo_item_type				m_type;
		QTVOSM::viewer_interface *	m_pVi;
		QString						m_name;
	protected:
		void			setLevel		(int currlevel);
		QTVOSM::viewer_interface *	vi(){return m_pVi;}
	public:
		int				level			()					{return m_nCurrentLevel;}
		geo_item_type	item_type		() const			{return m_type;}
		void			set_item_type	(geo_item_type tp)	{m_type = tp;}
		QString			item_name		() const			{return m_name;}
		void			set_item_name	(QString na)		{m_name = na;}
		void			setLabelFont	(QFont font)		{m_LabelTextFont = font;if (m_pLabelItem) m_pLabelItem->setFont(m_LabelTextFont);}
		QFont			labelFont		()					{return m_LabelTextFont;}
		void			setLabelColor	(QColor col)		{m_LabelTextColor = col;if (m_pLabelItem) m_pLabelItem->setDefaultTextColor(m_LabelTextColor);}
		QColor			labelColor		()					{return m_LabelTextColor;}
		void			adjustLabelPos	();

		QStringList			prop_names();
		QVariantList		prop_values();
		int					prop_counts();
		const QVariant &	prop_data		(const QString &);
		void				set_prop_data	(const QString &, const QVariant &);
		void				del_prop		(const QString & s);
		void				show_props		(bool bShow);
		bool				props_visible	();
	public:
		virtual void adjust_coords(int nNewLevel) = 0;
		virtual QPointF label_pos() = 0;
	};
}
#endif // GEOITEMBASE_H
