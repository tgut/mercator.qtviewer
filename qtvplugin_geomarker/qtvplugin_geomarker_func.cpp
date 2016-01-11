#include "qtvplugin_geomarker.h"
#include "ui_qtvplugin_geomarker.h"
#include "geographicsellipseitem.h"
#include "geographicslineitem.h"
#include "geographicspolygonitem.h"
#include "geographicsrectitem.h"
#include <QDebug>
#include <QMap>
/**
 * geomarker supports these function calls.
 *
 *
 *	ATTENTION!THIS FUNCTION IS NOT THREAD SAFE! Calls should be made only in main thread, also called "UI Thread"
 * Supported fucntions:
 * 1.function=update_point
 * 2.function=update_line
 * 3.function=update_polygon
 * 4.function=update_props
 * 5.function=exists
 * @param paras	the key-value style paraments.
 * @return QMap<QString, QVariant>	the key-value style return values.
 */
QMap<QString, QVariant> qtvplugin_geomarker::call_func(const  QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	if (paras.contains("function"))
	{
		QString funct = paras["function"].toString();
		if (m_map_pluginFunctions.contains(funct))
			res = m_map_pluginFunctions[funct](paras);
		else
			res["error"] = QString("unknown function \"%1\".").arg(funct);
	}
	else
		res["error"] = "\"function\" keyword not specified, nothing to do.";
	return std::move(res);
}

void qtvplugin_geomarker::initialBindPluginFuntions()
{
	m_map_pluginFunctions["update_point"] = std::bind(&qtvplugin_geomarker::func_update_point,this,std::placeholders::_1);
	m_map_pluginFunctions["update_line"] = std::bind(&qtvplugin_geomarker::func_update_line,this,std::placeholders::_1);
	m_map_pluginFunctions["update_polygon"] = std::bind(&qtvplugin_geomarker::func_update_polygon,this,std::placeholders::_1);
	m_map_pluginFunctions["update_props"] = std::bind(&qtvplugin_geomarker::func_update_props,this,std::placeholders::_1);
	m_map_pluginFunctions["exists"] = std::bind(&qtvplugin_geomarker::func_exists,this,std::placeholders::_1);
	m_map_pluginFunctions["delete_marks"] = std::bind(&qtvplugin_geomarker::func_delete_marks,this,std::placeholders::_1);
	m_map_pluginFunctions["delete_props"] = std::bind(&qtvplugin_geomarker::func_delete_props,this,std::placeholders::_1);
	m_map_pluginFunctions["mark_names"] = std::bind(&qtvplugin_geomarker::func_mark_names,this,std::placeholders::_1);
	m_map_pluginFunctions["mark"] = std::bind(&qtvplugin_geomarker::func_mark,this,std::placeholders::_1);
}

QMap<QString, QVariant> qtvplugin_geomarker::func_update_point		(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	if (paras.contains("name")==false || paras.contains("lat")==false || paras.contains("lon")==false)
	{
		res["error"] = tr("name, lat, lon must  exist in paraments.");
		return std::move(res);

	}
	QString name = paras["name"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("name could not be empty.");
		return std::move(res);
	}
	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	QPen pen(Qt::SolidLine);
	QBrush brush(QColor(255,255,255,128));
	qreal width =8;
	qreal height =8;

	if (base)
	{
		if (base->item_type()==QTVP_GEOMARKER::ITEAMTYPE_RECT_POINT)
		{
			QTVP_GEOMARKER::geoGraphicsRectItem * it = dynamic_cast<QTVP_GEOMARKER::geoGraphicsRectItem * >(base);
			if (it)
			{
				pen = it->pen();
				brush = it->brush();
				width = it->width();
				height = it->height();
			}
		}
		else if (base->item_type()==QTVP_GEOMARKER::ITEAMTYPE_ELLIPSE_POINT)
		{
			QTVP_GEOMARKER::geoGraphicsEllipseItem * it = dynamic_cast<QTVP_GEOMARKER::geoGraphicsEllipseItem * >(base);
			if (it)
			{
				pen = it->pen();
				brush = it->brush();
				width = it->width();
				height = it->height();
			}
		}
	}
	if ( paras.contains("style_pen"))
	{
		//Get pen and brush settings
		Qt::PenStyle pst [] ={
			Qt::NoPen	,
			Qt::SolidLine	,
			Qt::DashLine	,
			Qt::DotLine	,
			Qt::DashDotLine	,
			Qt::DashDotDotLine	,
			Qt::CustomDashLine
		};
		int ptdd =paras["style_pen"].toInt();
		if (ptdd < 0 || ptdd >=7)
			ptdd = 1;
		pen.setStyle(pst[ptdd]);
	}
	if ( paras.contains("color_pen"))
	{
		QColor penColor = string2color( paras["color_pen"].toString());
		pen.setColor(penColor);

	}
	if ( paras.contains("width_pen"))
	{
		int penWidth =paras["width_pen"].toInt();
		if (penWidth<0)	penWidth = 1;
		pen.setWidth(penWidth);

	}

	if ( paras.contains("style_brush"))
	{
		Qt::BrushStyle bst [] = {
			Qt::NoBrush,
			Qt::SolidPattern,
			Qt::Dense1Pattern,
			Qt::Dense2Pattern,
			Qt::Dense3Pattern,
			Qt::Dense4Pattern,
			Qt::Dense5Pattern,
			Qt::Dense6Pattern,
			Qt::Dense7Pattern,
			Qt::HorPattern,
			Qt::VerPattern,
			Qt::CrossPattern,
			Qt::BDiagPattern,
			Qt::FDiagPattern,
			Qt::DiagCrossPattern
		};
		int btdd = paras["style_brush"].toInt();
		if (btdd < 0 || btdd >=15)
			btdd = 1;
		brush.setStyle(bst[btdd]);
	}
	if ( paras.contains("color_brush"))
	{
		QColor brushColor = string2color( paras["color_brush"].toString());
		brush.setColor(brushColor);
	}

	if ( paras.contains("width"))
	{
		width =paras["width"].toReal();
		if (width ==0)	width = 8;
	}
	if ( paras.contains("height"))
	{
		height =paras["height"].toReal();
		if (height ==0)	height = 8;
	}
	QTVP_GEOMARKER::geoItemBase * newitem = 0;

	double lat =paras["lat"].toDouble();
	double lon = paras["lon"].toDouble();

	int tpn = paras["type"].toInt();
	if (tpn > 1 || tpn <0) tpn = 0;
	QTVP_GEOMARKER::geo_item_type tpe = static_cast< QTVP_GEOMARKER::geo_item_type > (tpn);
	if (tpe==QTVP_GEOMARKER::ITEAMTYPE_RECT_POINT)
		newitem = update_point<QTVP_GEOMARKER::geoGraphicsRectItem>(name,lat,lon,width,height,pen,brush);
	else
		newitem = update_point<QTVP_GEOMARKER::geoGraphicsEllipseItem>(name,lat,lon,width,height,pen,brush);
	if (newitem)
	{
		QFont f = newitem->labelFont();
		if ( paras.contains("size_label"))
		{
			int fontSz = paras["size_label"].toInt();
			if (fontSz==0)	fontSz = 9;
			f.setPointSize(fontSz);
		}
		if ( paras.contains("weight_label"))
		{
			int fontWeight = paras["weight_label"].toInt();
			f.setWeight(fontWeight);
		}
		newitem->setLabelFont(f);
		if ( paras.contains("color_label"))
		{
			QColor textColor = string2color( paras["color_label"].toString());
			newitem->setLabelColor(textColor);
		}
		scheduleRefreshMarks();
		scheduleUpdateMap();
	}
	else
		res["error"] = tr("can not create graphical object, the pointer is zero.");
	return std::move(res);
}
QMap<QString, QVariant>  qtvplugin_geomarker::func_update_line		(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	if (paras.contains("name")==false || paras.contains("lat0")==false || paras.contains("lon0")==false
			|| paras.contains("lat1")==false || paras.contains("lon1")==false)
	{
		res["error"] = tr("name, lat0, lon0,lat1,lon1 must  exist in paraments.");
		return std::move(res);
	}

	QString name = paras["name"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("name could not be empty.");
		return std::move(res);
	}
	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	QPen pen(Qt::SolidLine);
	if (base)
	{
		if (base->item_type()==QTVP_GEOMARKER::ITEAMTYPE_LINE)
		{
			QTVP_GEOMARKER::geoGraphicsLineItem * it = dynamic_cast<QTVP_GEOMARKER::geoGraphicsLineItem * >(base);
				pen = it->pen();
		}

	}
	if ( paras.contains("style_pen"))
	{
		//Get pen and brush settings
		Qt::PenStyle pst [] ={
			Qt::NoPen	,
			Qt::SolidLine	,
			Qt::DashLine	,
			Qt::DotLine	,
			Qt::DashDotLine	,
			Qt::DashDotDotLine	,
			Qt::CustomDashLine
		};
		int ptdd =paras["style_pen"].toInt();
		if (ptdd < 0 || ptdd >=7)
			ptdd = 1;
		pen.setStyle(pst[ptdd]);
	}
	if ( paras.contains("color_pen"))
	{
		QColor penColor = string2color( paras["color_pen"].toString());
		pen.setColor(penColor);

	}
	if ( paras.contains("width_pen"))
	{
		int penWidth =paras["width_pen"].toInt();
		if (penWidth<0)	penWidth = 1;
		pen.setWidth(penWidth);

	}

	QTVP_GEOMARKER::geoItemBase * newitem = 0;

	double lat1 =paras["lat0"].toDouble();
	double lon1 = paras["lon0"].toDouble();
	double lat2 =paras["lat1"].toDouble();
	double lon2 = paras["lon1"].toDouble();

	newitem = update_line(name,lat1,lon1,lat2,lon2,pen);
	if (newitem)
	{
		QFont f = newitem->labelFont();
		if ( paras.contains("size_label"))
		{
			int fontSz =paras["size_label"].toInt();
			if (fontSz==0)	fontSz = 9;
			f.setPointSize(fontSz);
		}
		if ( paras.contains("weight_label"))
		{
			int fontWeight = paras["weight_label"].toInt();
			f.setWeight(fontWeight);
		}
		newitem->setLabelFont(f);
		if ( paras.contains("color_label"))
		{
			QColor textColor = string2color( paras["color_label"].toString());
			newitem->setLabelColor(textColor);
		}
		scheduleRefreshMarks();
		scheduleUpdateMap();
	}
	else
		res["error"] = tr("can not create graphical object, the pointer is zero.");
	return std::move(res);

}
QMap<QString, QVariant> qtvplugin_geomarker::func_update_polygon		(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;

	if (paras.contains("name")==false)
	{
		res["error"] = tr("name must  exist in paraments.");
		return std::move(res);
	}


	QString name = paras["name"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("name could not be empty.");
		return std::move(res);
	}
	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	QPen pen(Qt::SolidLine);
	QBrush brush(QColor(255,255,255,128));
	if (base)
	{
		if (base->item_type()==QTVP_GEOMARKER::ITEAMTYPE_POLYGON)
		{
			QTVP_GEOMARKER::geoGraphicsPolygonItem * it = dynamic_cast<QTVP_GEOMARKER::geoGraphicsPolygonItem * >(base);
			if (it)
			{
				pen = it->pen();
				brush = it->brush();
			}
		}

	}
	if ( paras.contains("style_pen"))
	{
		//Get pen and brush settings
		Qt::PenStyle pst [] ={
			Qt::NoPen	,
			Qt::SolidLine	,
			Qt::DashLine	,
			Qt::DotLine	,
			Qt::DashDotLine	,
			Qt::DashDotDotLine	,
			Qt::CustomDashLine
		};
		int ptdd =paras["style_pen"].toInt();
		if (ptdd < 0 || ptdd >=7)
			ptdd = 1;
		pen.setStyle(pst[ptdd]);
	}
	if ( paras.contains("color_pen"))
	{
		QColor penColor = string2color( paras["color_pen"].toString());
		pen.setColor(penColor);

	}
	if ( paras.contains("width_pen"))
	{
		int penWidth =paras["width_pen"].toInt();
		if (penWidth<0)	penWidth = 1;
		pen.setWidth(penWidth);

	}

	if ( paras.contains("style_brush"))
	{
		Qt::BrushStyle bst [] = {
			Qt::NoBrush,
			Qt::SolidPattern,
			Qt::Dense1Pattern,
			Qt::Dense2Pattern,
			Qt::Dense3Pattern,
			Qt::Dense4Pattern,
			Qt::Dense5Pattern,
			Qt::Dense6Pattern,
			Qt::Dense7Pattern,
			Qt::HorPattern,
			Qt::VerPattern,
			Qt::CrossPattern,
			Qt::BDiagPattern,
			Qt::FDiagPattern,
			Qt::DiagCrossPattern
		};
		int btdd = paras["style_brush"].toInt();
		if (btdd < 0 || btdd >=15)
			btdd = 1;
		brush.setStyle(bst[btdd]);
	}
	if ( paras.contains("color_brush"))
	{
		QColor brushColor = string2color( paras["color_brush"].toString());
		brush.setColor(brushColor);
	}

	QTVP_GEOMARKER::geoItemBase * newitem = 0;

	QPolygonF pl;
	int ct = 0;
	QString strKeyLat = QString("lat%1").arg(ct);
	QString strKeyLon = QString("lon%1").arg(ct);
	while (paras.contains(strKeyLat) && paras.contains(strKeyLon))
	{
		double lat =paras[strKeyLat].toDouble();
		double lon = paras[strKeyLon].toDouble();
		pl.push_back(QPointF(lon,lat));
		++ct;
		strKeyLat = QString("lat%1").arg(ct);
		strKeyLon = QString("lon%1").arg(ct);
	}
	if (pl.size()>2)
		newitem = update_polygon(name,pl,pen,brush);
	else
		res["error"] = tr("polygon must contain at least 3 points,lat0,lat1,lat2 and lon0,lon1, lon2.");

	if (newitem)
	{
		QFont f = newitem->labelFont();
		if ( paras.contains("size_label"))
		{
			int fontSz = paras["size_label"].toInt();
			if (fontSz==0)	fontSz = 9;
			f.setPointSize(fontSz);
		}
		if ( paras.contains("weight_label"))
		{
			int fontWeight = paras["weight_label"].toInt();
			f.setWeight(fontWeight);
		}
		newitem->setLabelFont(f);
		if ( paras.contains("color_label"))
		{
			QColor textColor = string2color( paras["color_label"].toString());
			newitem->setLabelColor(textColor);
		}
		scheduleRefreshMarks();
		scheduleUpdateMap();
	}
	else
		res["error"] = tr("can not create graphical object, the pointer is zero.");
	return std::move(res);

}
QMap<QString, QVariant> qtvplugin_geomarker::func_update_props(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	QString name = paras["name"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("name does not exist in paraments.");
		return std::move(res);
	}

	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	if (base)
	{
		foreach (QString key , paras.keys())
		{
			if (key!="name" && key!="function")
				base->set_prop_data(key,paras[key]);
		}
		scheduleRefreshMarks();
		scheduleUpdateMap();
	}
	else
		res["error"] = tr("name does not exist in scene.");

	return std::move(res);
}
QMap<QString, QVariant>			qtvplugin_geomarker::func_exists		(const QMap<QString, QVariant>  & paras)
{
	QMap<QString, QVariant> res;
	QString name = paras["name"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("name does not exist in paraments.");
		return std::move(res);
	}
	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	if (base)
	{
		res["return"] = 1;
	}
	else
		res["return"] = 0;
	return std::move(res);
}
QMap<QString, QVariant>			qtvplugin_geomarker::func_delete_marks	(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;

	QSet<QString> set_names;
	int ct = 0;
	do{
		QString keystr = QString("name%1").arg(ct++);
		if (paras.contains(keystr)==false)
			break;
		set_names.insert(paras[keystr].toString());
	}while (ct<1024*1024*1024);
	if (set_names.size())
	{
		ct = 0;
		foreach (QString key,set_names)
		{
			QString keystr = QString("name%1").arg(ct++);
			QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(key);
			if (base)
			{
				m_pScene->removeItem(base,0);
				res[keystr] = 1;
			}
			else
				res[keystr] = 0;
		}
	}
	else
	{
		QList< QTVP_GEOMARKER::geoItemBase *  > lst = m_pScene->geo_items();
		foreach (QTVP_GEOMARKER::geoItemBase * key,lst)
			m_pScene->removeItem(key,0);
		res["ALL"] = 1;
	}

	return std::move(res);
}

QMap<QString, QVariant>			qtvplugin_geomarker::func_delete_props	(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	if (paras.contains("name")==false)
	{
		res["error"] = tr("name must  exist in paraments.");
		return std::move(res);
	}
	QString name = paras["name"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("name could not be empty.");
		return std::move(res);
	}
	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	if (base)
	{
		int ct = 0;
		do{
			QString keystr = QString("prop%1").arg(ct++);
			if (paras.contains(keystr)==false)
				break;
			QString prop_name = paras[keystr].toString();
			base->del_prop(prop_name);
		}while (ct<1024*1024*1024);
	}
	else
		res["error"] = tr("the mark name.") + name + tr(" does not exist in current scene.");

	return std::move(res);
}

QMap<QString, QVariant>			qtvplugin_geomarker::func_mark_names		(const QMap<QString, QVariant> & /*paras*/)
{
	QMap<QString, QVariant> res;
	QList< QTVP_GEOMARKER::geoItemBase *  > lst = m_pScene->geo_items();
	int ct = 0;
	foreach (QTVP_GEOMARKER::geoItemBase * key,lst)
	{
		QString keystr = QString("name%1").arg(ct++);
		res[keystr] = key->item_name();
	}
	return std::move(res);
}

QMap<QString, QVariant>			qtvplugin_geomarker::func_mark			(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	return std::move(res);
}
