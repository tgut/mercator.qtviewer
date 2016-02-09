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
 * 1.update_point		Insert or Update a point mark.
 * 2.update_line		Insert or Update a line mark.
 * 3.update_polygon		Insert or Update a polygon mark.
 * 4.update_icon		Insert or Update a icon mark.
 * 5.update_props		Insert or Update a mark's user-defind properties.
 * 6.exists				Test whether a special mark is exist.
 * 7.delete_marks		Delete marks.
 * 8.delete_props		Delete user-defined properties for a mark.
 * 9.mark_names			return All mark names owned by this plugin.
 * 10.mark				return All styles and geo points for a special mark.
 * 11.props				return All user-defined properties for a special mark.
 * 12.add_reource		add an reource (eg, icon) to current resource list. icon mark can reference icons in current resource list.
 * 13 save_reources		save current resource list to disk files.
 * 14 load_reources		load previewsly saved list file from disk to current resource list.
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

/**
 * @brief initialBindPluginFuntions using C++11/14 style of function bindings to build a
 * string map. this approach is faster than if-elseif switches when there are a lot if functions to maintain.
 */
void qtvplugin_geomarker::initialBindPluginFuntions()
{
	m_map_pluginFunctions["update_point"]	= std::bind(&qtvplugin_geomarker::func_update_point,	this,std::placeholders::_1);
	m_map_pluginFunctions["update_line"]	= std::bind(&qtvplugin_geomarker::func_update_line,		this,std::placeholders::_1);
	m_map_pluginFunctions["update_polygon"]	= std::bind(&qtvplugin_geomarker::func_update_polygon,	this,std::placeholders::_1);
	m_map_pluginFunctions["update_icon"]	= std::bind(&qtvplugin_geomarker::func_update_icon,		this,std::placeholders::_1);
	m_map_pluginFunctions["update_props"]	= std::bind(&qtvplugin_geomarker::func_update_props,	this,std::placeholders::_1);
	m_map_pluginFunctions["exists"]			= std::bind(&qtvplugin_geomarker::func_exists,			this,std::placeholders::_1);
	m_map_pluginFunctions["delete_marks"]	= std::bind(&qtvplugin_geomarker::func_delete_marks,	this,std::placeholders::_1);
	m_map_pluginFunctions["delete_props"]	= std::bind(&qtvplugin_geomarker::func_delete_props,	this,std::placeholders::_1);
	m_map_pluginFunctions["mark_names"]		= std::bind(&qtvplugin_geomarker::func_mark_names,		this,std::placeholders::_1);
	m_map_pluginFunctions["mark"]			= std::bind(&qtvplugin_geomarker::func_mark,			this,std::placeholders::_1);
	m_map_pluginFunctions["props"]			= std::bind(&qtvplugin_geomarker::func_props,			this,std::placeholders::_1);
	m_map_pluginFunctions["load_xml"]		= std::bind(&qtvplugin_geomarker::func_load_xml,		this,std::placeholders::_1);
	m_map_pluginFunctions["save_xml"]		= std::bind(&qtvplugin_geomarker::func_save_xml,		this,std::placeholders::_1);
	m_map_pluginFunctions["add_reource"]	= std::bind(&qtvplugin_geomarker::func_add_reource,		this,std::placeholders::_1);
	m_map_pluginFunctions["save_reources"]	= std::bind(&qtvplugin_geomarker::func_save_reources,	this,std::placeholders::_1);
	m_map_pluginFunctions["load_reources"]	= std::bind(&qtvplugin_geomarker::func_load_reources,	this,std::placeholders::_1);
}

/**
 * @brief func_update_point is a internal function for plugin call_func "update_point"
 *
 * the paraments used by paras is listed below.
 * function=update_point;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant> if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant> qtvplugin_geomarker::func_update_point		(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	//!name, lat, lon has no default values. user should specify these values or the function calll will fail;
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
	//if the mark is already exist, we will get its orgional style as default .
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

	//! style_pen from 0~6, is corresponds to the pen combo-box in UI system.
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
	//! color_pen has 4 pen color band values splitted by comma, r,g,b,a
	if ( paras.contains("color_pen"))
	{
		QColor penColor = string2color( paras["color_pen"].toString());
		pen.setColor(penColor);

	}
	//! width_pen has a value >0 , stand for the point width of the pen on screen.
	if ( paras.contains("width_pen"))
	{
		int penWidth =paras["width_pen"].toInt();
		if (penWidth<0)	penWidth = 1;
		pen.setWidth(penWidth);

	}
	//! style_brush from 0~14, is corresponds to the brush style combo-box in UI system.
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

	//! color_brush has 4 brush color band values splitted by comma, r,g,b,a
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
	/*! geo coordinate in WGS84 lattitude, longitude should be saved as lat and lon.
	*/
	double lat =paras["lat"].toDouble();
	double lon = paras["lon"].toDouble();

	int tpn = paras["type"].toInt();
	if (tpn > 2 || tpn <1) tpn = 1;
	QTVP_GEOMARKER::geo_item_type tpe = static_cast< QTVP_GEOMARKER::geo_item_type > (tpn);
	//update using same function in UI
	if (tpe==QTVP_GEOMARKER::ITEAMTYPE_RECT_POINT)
		newitem = update_point<QTVP_GEOMARKER::geoGraphicsRectItem>(name,lat,lon,width,height,pen,brush);
	else
		newitem = update_point<QTVP_GEOMARKER::geoGraphicsEllipseItem>(name,lat,lon,width,height,pen,brush);
	if (newitem)
	{
		QFont f = newitem->labelFont();
		//! size_label stands for the text label font pixel size from 1 - 720, with a normal value 9.
		if ( paras.contains("size_label"))
		{
			int fontSz = paras["size_label"].toInt();
			if (fontSz==0)	fontSz = 9;
			f.setPointSize(fontSz);
		}
		//! weight_label is the bolder rate for  text renderring, from 1 ~ 99, 99 is the heaviest.
		if ( paras.contains("weight_label"))
		{
			int fontWeight = paras["weight_label"].toInt();
			f.setWeight(fontWeight);
		}
		newitem->setLabelFont(f);
		//! color_label has 4 text color band values splitted by comma, r,g,b,a
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
/**
 * @brief func_update_icon is a internal function for plugin call_func "update_icon"
 *
 * the paraments used by paras is listed below.
 * function=update_icon;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant> if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker:: func_update_icon	(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	//!name, lat, lon has no default values. user should specify these values or the function calll will fail;
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
	QString icon_name = "default";
	qreal scale = 1.0;
	qreal rot = 0.0;
	int smooth = 0;
	//if the mark is already exist, we will get its orgional style as default .
	if (base)
	{
		QTVP_GEOMARKER::geoGraphicsPixmapItem * it = dynamic_cast<QTVP_GEOMARKER::geoGraphicsPixmapItem * >(base);
		if (it)
		{
			icon_name = it->icon()->name;
			scale = it->scale();
			rot = it->rotation();
			smooth = it->transformationMode()==Qt::SmoothTransformation?1:0;
		}
	}

	//! icon is the name that this mark will use.
	if ( paras.contains("icon"))
	{
		QString icn = paras["icon"].toString();
		if (icn.length())	icon_name = icn;
	}
	//! scale is the zoom ratio that this icon will use, 1.0 means no zoom
	if ( paras.contains("scale"))
	{
		qreal sc = paras["scale"].toReal();
		if (sc >0 )	scale = sc;
	}
	//! rotate is the rotate angle that this icon will use, 0.0 means no rotate
	if ( paras.contains("rotate"))
	{
		qreal rt = paras["rotate"].toReal();
		rot = rt;
	}
	//!smooth is the transform mode that this icon will use. 0 mean not smooth, but faster. 1 mean smooth.
	if ( paras.contains("smooth"))
	{
		int smt =paras["smooth"].toInt();
		if (smt ==0)	smooth = 0; else smooth = 1;
	}
	QTVP_GEOMARKER::geoItemBase * newitem = 0;
	/*! geo coordinate in WGS84 lattitude, longitude should be saved as lat and lon.
	*/
	double lat =paras["lat"].toDouble();
	double lon = paras["lon"].toDouble();

	newitem = update_icon(name,lat,lon,scale,rot,smooth,icon_name);
	if (newitem)
	{
		QFont f = newitem->labelFont();
		//! size_label stands for the text label font pixel size from 1 - 720, with a normal value 9.
		if ( paras.contains("size_label"))
		{
			int fontSz = paras["size_label"].toInt();
			if (fontSz==0)	fontSz = 9;
			f.setPointSize(fontSz);
		}
		//! weight_label is the bolder rate for  text renderring, from 1 ~ 99, 99 is the heaviest.
		if ( paras.contains("weight_label"))
		{
			int fontWeight = paras["weight_label"].toInt();
			f.setWeight(fontWeight);
		}
		newitem->setLabelFont(f);
		//! color_label has 4 text color band values splitted by comma, r,g,b,a
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

/**
 * @brief func_update_line is a internal function for plugin call_func "update_line"
 *
 * the paraments used by paras is listed below.
 * function=update_line;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>  qtvplugin_geomarker::func_update_line		(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	//!name, lat0, lon0, lat1,lon1 has no default values. user should specify these values or the function calll will fail;
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

	//if the mark is already exist, we will get its orgional style as default .
	if (base)
	{
		if (base->item_type()==QTVP_GEOMARKER::ITEAMTYPE_LINE)
		{
			QTVP_GEOMARKER::geoGraphicsLineItem * it = dynamic_cast<QTVP_GEOMARKER::geoGraphicsLineItem * >(base);
				pen = it->pen();
		}

	}

	//! style_pen from 0~6, is corresponds to the pen combo-box in UI system.
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
	//! color_pen has 4 pen color band values splitted by comma, r,g,b,a
	if ( paras.contains("color_pen"))
	{
		QColor penColor = string2color( paras["color_pen"].toString());
		pen.setColor(penColor);

	}
	//! width_pen has a value >0 , stand for the point width of the pen on screen.
	if ( paras.contains("width_pen"))
	{
		int penWidth =paras["width_pen"].toInt();
		if (penWidth<0)	penWidth = 1;
		pen.setWidth(penWidth);

	}

	QTVP_GEOMARKER::geoItemBase * newitem = 0;
	/*! geo coordinates in WGS84 lattitude, longitude should be saved like this:
	 * lat0, lon0 stand for point1, lat1,lon1 stand for point2
	*/
	double lat1 =paras["lat0"].toDouble();
	double lon1 = paras["lon0"].toDouble();
	double lat2 =paras["lat1"].toDouble();
	double lon2 = paras["lon1"].toDouble();
	//update using same function in UI
	newitem = update_line(name,lat1,lon1,lat2,lon2,pen);
	if (newitem)
	{
		QFont f = newitem->labelFont();
		//! size_label stands for the text label font pixel size from 1 - 720, with a normal value 9.
		if ( paras.contains("size_label"))
		{
			int fontSz =paras["size_label"].toInt();
			if (fontSz==0)	fontSz = 9;
			f.setPointSize(fontSz);
		}
		//! weight_label is the bolder rate for  text renderring, from 1 ~ 99, 99 is the heaviest.
		if ( paras.contains("weight_label"))
		{
			int fontWeight = paras["weight_label"].toInt();
			f.setWeight(fontWeight);
		}
		newitem->setLabelFont(f);
		//! color_label has 4 text color band values splitted by comma, r,g,b,a
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

/**
 * @brief func_update_polygon is a internal function for plugin call_func "update_polygo"
 *
 * the paraments used by paras is listed below.
 * function=update_polygo;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant> qtvplugin_geomarker::func_update_polygon		(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	//!name, latn, lonn, has no default values, n>=3. user should specify these values or the function calll will fail;
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

	//if the mark is already exist, we will get its orgional style as default .
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

	//! style_pen from 0~6, is corresponds to the pen combo-box in UI system.
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

	//! color_pen has 4 pen color band values splitted by comma, r,g,b,a
	if ( paras.contains("color_pen"))
	{
		QColor penColor = string2color( paras["color_pen"].toString());
		pen.setColor(penColor);

	}
	//! width_pen has a value >0 , stand for the point width of the pen on screen.
	if ( paras.contains("width_pen"))
	{
		int penWidth =paras["width_pen"].toInt();
		if (penWidth<0)	penWidth = 1;
		pen.setWidth(penWidth);

	}
	//! style_brush from 0~14, is corresponds to the brush style combo-box in UI system.
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
	//! color_brush has 4 brush color band values splitted by comma, r,g,b,a
	if ( paras.contains("color_brush"))
	{
		QColor brushColor = string2color( paras["color_brush"].toString());
		brush.setColor(brushColor);
	}

	QTVP_GEOMARKER::geoItemBase * newitem = 0;
	/*! geo coordinates in WGS84 lattitude, longitude should be saved like this:
	 * lat0, lon0 stand for point1, lat1,lon1 stand for point2,latn-1, lonn-1 stand for point n.
	*/
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
	//update using same function in UI
	if (pl.size()>2)
		newitem = update_polygon(name,pl,pen,brush);
	else
		res["error"] = tr("polygon must contain at least 3 points,lat0,lat1,lat2 and lon0,lon1, lon2.");

	if (newitem)
	{
		QFont f = newitem->labelFont();
		//! size_label stands for the text label font pixel size from 1 - 720, with a normal value 9.
		if ( paras.contains("size_label"))
		{
			int fontSz = paras["size_label"].toInt();
			if (fontSz==0)	fontSz = 9;
			f.setPointSize(fontSz);
		}
		//! weight_label is the bolder rate for  text renderring, from 1 ~ 99, 99 is the heaviest.
		if ( paras.contains("weight_label"))
		{
			int fontWeight = paras["weight_label"].toInt();
			f.setWeight(fontWeight);
		}
		newitem->setLabelFont(f);
		//! color_label has 4 text color band values splitted by comma, r,g,b,a
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

/**
 * @brief func_update_props is a internal function for plugin call_func "update_props"
 *
 * the paraments used by paras is listed below.
 * function=update_props;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant> qtvplugin_geomarker::func_update_props(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	//!name: user should specify these values or the function calll will fail;
	QString name = paras["name"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("name does not exist in paraments.");
		return std::move(res);
	}
	/** the user-defined properties are defined as key-value pairs.
	 *  [prop_name]=[prop_value]
	 *  note that plugin keyword "name" and "function" should not be used.
	 */
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

/**
 * @brief func_exists is a internal function for plugin call_func "exists"
 *
 * the paraments used by paras is listed below.
 * function=exists;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_exists		(const QMap<QString, QVariant>  & paras)
{
	QMap<QString, QVariant> res;
	//!name: user should specify these values or the function calll will fail;
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
	//! return 1 means the mark name is exist, 0 mean not.
	return std::move(res);
}

/**
 * @brief func_delete_marks is a internal function for plugin call_func "delete_marks"
 *
 * the paraments used by paras is listed below.
 * function=delete_marks;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_delete_marks	(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	//! marknames should stored in parename name0, name1, name2
	//! for example, name0=StarBar;name1=ruijing Hospital;name2=shunfun express;
	QSet<QString> set_names;
	int ct = 0;
	do{
		QString keystr = QString("name%1").arg(ct++);
		if (paras.contains(keystr)==false)
			break;
		set_names.insert(paras[keystr].toString());
	}while (ct<1024*1024*1024);
	bool needUpdate = false;
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
				needUpdate = true;
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
	if (needUpdate)
	{
		scheduleRefreshMarks();
		scheduleUpdateMap();
	}

	return std::move(res);
}
/**
 * @brief func_delete_props is a internal function for plugin call_func "delete_props"
 *
 * the paraments used by paras is listed below.
 * function=delete_props;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_delete_props	(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	//!name: user should specify these values or the function calll will fail;
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
	/*!
	 * the property names to be deleted will be stored like this:
	 * prop0, prop1,...,propn-1
	 * for example:
	 * prop0=TIME;prop1=Profit;Prop2=Address;Prop3=tel;
	 */
	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	bool needUpdate = false;
	if (base)
	{
		int ct = 0;
		do{
			QString keystr = QString("prop%1").arg(ct++);
			if (paras.contains(keystr)==false)
				break;
			QString prop_name = paras[keystr].toString();
			base->del_prop(prop_name);
			needUpdate = true;
		}while (ct<1024*1024*1024);
	}
	else
		res["error"] = tr("the mark name.") + name + tr(" does not exist in current scene.");

	if (needUpdate)
	{
		scheduleRefreshMarks();
		scheduleUpdateMap();
	}
	return std::move(res);
}

/**
 * @brief func_mark_names is a internal function for plugin call_func "mark_names"
 *
 * the paraments used by paras is listed below.
 * function=func_mark_names;
 * @param paras The key-value style paraments. no other paras needed except for "function"
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
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
	//! the mark names will be stored in key-value pairs, with
	//! name0=??;name1=??;name2=??...namen-1=??
	return std::move(res);
}
/**
 * @brief func_mark is a internal function for plugin call_func "mark"
 *
 * the paraments used by paras is listed below.
 * function=mark;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_mark			(const QMap<QString, QVariant> & paras)
{
	//!name: user should specify these values or the function calll will fail;
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
	QTVP_GEOMARKER::geoItemBase * item = m_pScene->geoitem_by_name(name);
	if (item)
	{
		QTVP_GEOMARKER::geo_item_type x_tp = item->item_type();
		QString x_name = item->item_name();

		//! in return parament, contains same keys as "func_update_point","func_update_line",and "func_update_polygon":

		res["name"] = x_name;
		res["type"] = QString("%1").arg((int)x_tp);
		switch (x_tp)
		{
		case QTVP_GEOMARKER::ITEAMTYPE_RECT_POINT:
		{
			QTVP_GEOMARKER::geoGraphicsRectItem * pU = dynamic_cast<QTVP_GEOMARKER::geoGraphicsRectItem *>(item);
			if (pU)
			{
				res["lat"] = QString("%1").arg(pU->lat(),0,'f',7);
				res["lon"] = QString("%1").arg(pU->lon(),0,'f',7);
				res["width"] = QString("%1").arg(pU->width());
				res["height"] = QString("%1").arg(pU->height());
				res["color_pen"] = color2string(pU->pen().color());
				res["style_pen"] = QString("%1").arg(int(pU->pen().style()));
				res["width_pen"] = QString("%1").arg(int(pU->pen().width()));
				res["color_brush"] = color2string(pU->brush().color());
				res["style_brush"] = QString("%1").arg(int(pU->brush().style()));

			}
		}
			break;
		case QTVP_GEOMARKER::ITEAMTYPE_ELLIPSE_POINT:
		{
			QTVP_GEOMARKER::geoGraphicsEllipseItem * pU = dynamic_cast<QTVP_GEOMARKER::geoGraphicsEllipseItem *>(item);
			if (pU)
			{
				res["lat"] = QString("%1").arg(pU->lat(),0,'f',7);
				res["lon"] = QString("%1").arg(pU->lon(),0,'f',7);
				res["width"] = QString("%1").arg(pU->width());
				res["height"] = QString("%1").arg(pU->height());
				res["color_pen"] = color2string(pU->pen().color());
				res["style_pen"] = QString("%1").arg(int(pU->pen().style()));
				res["width_pen"] = QString("%1").arg(int(pU->pen().width()));
				res["color_brush"] = color2string(pU->brush().color());
				res["style_brush"] = QString("%1").arg(int(pU->brush().style()));
			}
		}
			break;
		case QTVP_GEOMARKER::ITEAMTYPE_LINE:
		{
			QTVP_GEOMARKER::geoGraphicsLineItem * pU = dynamic_cast<QTVP_GEOMARKER::geoGraphicsLineItem *>(item);
			if (pU)
			{
				res["lat0"] = QString("%1").arg(pU->lat1(),0,'f',7);
				res["lon0"] = QString("%1").arg(pU->lon1(),0,'f',7);
				res["lat1"] = QString("%1").arg(pU->lat2(),0,'f',7);
				res["lon1"] = QString("%1").arg(pU->lon2(),0,'f',7);
				res["color_pen"] = color2string(pU->pen().color());
				res["style_pen"] = QString("%1").arg(int(pU->pen().style()));
				res["width_pen"] = QString("%1").arg(int(pU->pen().width()));
			}
		}
			break;
		case QTVP_GEOMARKER::ITEAMTYPE_POLYGON:
		{
			QTVP_GEOMARKER::geoGraphicsPolygonItem * pU = dynamic_cast<QTVP_GEOMARKER::geoGraphicsPolygonItem *>(item);
			if (pU)
			{
				QPolygonF pl = pU->llas();
				int nPl = pl.size();
				for (int in = 0;in < nPl; ++in)
				{
					QString keyLat = QString("lat%1").arg(in);
					QString keyLon = QString("lon%1").arg(in);
					res[keyLat] = QString("%1").arg(pl[in].y(),0,'f',7);
					res[keyLon] = QString("%1").arg(pl[in].x(),0,'f',7);
				}
				res["color_pen"] = color2string(pU->pen().color());
				res["style_pen"] = QString("%1").arg(int(pU->pen().style()));
				res["width_pen"] = QString("%1").arg(int(pU->pen().width()));
				res["color_brush"] = color2string(pU->brush().color());
				res["style_brush"] = QString("%1").arg(int(pU->brush().style()));
			}
		}
			break;
		case QTVP_GEOMARKER::ITEAMTYPE_PIXMAP:
		{
			QTVP_GEOMARKER::geoGraphicsPixmapItem * pU = dynamic_cast<QTVP_GEOMARKER::geoGraphicsPixmapItem *>(item);
			if (pU)
			{
				res["lat"] = QString("%1").arg(pU->lat(),0,'f',7);
				res["lon"] = QString("%1").arg(pU->lon(),0,'f',7);
				res["icon"] = pU->icon()->name;
				res["scale"] =  pU->scale();
				res["rotate"] =  pU->rotation();
				res["smooth"] = pU->transformationMode()==Qt::SmoothTransformation?1:0;
			}
		}
			break;
		default:
			break;
		}

		QColor colorText = item->labelColor();
		res["color_label"] = color2string(colorText);

		int fsize = item->labelFont().pointSize();
		res["size_label"] = QString("%1").arg(fsize);

		int weight = item->labelFont().weight();
		res["weight_label"] = QString("%1").arg(weight);

	}
	else
		res["error"] = tr("the mark name.") + name + tr(" does not exist in current scene.");
	return std::move(res);
}
/**
 * @brief func_props is a internal function for plugin call_func "props"
 *
 * the paraments used by paras is listed below.
 * function=props;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_props			(const QMap<QString, QVariant> & paras)
{
	//!name: user should specify these values or the function calll will fail;
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
	QTVP_GEOMARKER::geoItemBase * item = m_pScene->geoitem_by_name(name);
	if (item)
	{
		QString x_name = item->item_name();
		res["name"] = x_name;
		int props = item->prop_counts();
		QStringList lstNames = item->prop_names();
		QVariantList lstValues = item->prop_values();
		for (int i=0;i<props;++i)
			res[lstNames[i]] = lstValues[i];

	}
	else
		res["error"] = tr("the mark name.") + name + tr(" does not exist in current scene.");
	//! return value is the user-defined key-pairs.
	return std::move(res);
}
/**
 * @brief func_save_xml is a internal function for plugin call_func "save_xml"
 *
 * the paraments used by paras is listed below.
 * function=save_xml;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_save_xml		(const QMap<QString, QVariant> & paras)
{
	//!xml: user should specify xml filename or the function calll will fail;
	QMap<QString, QVariant> res;
	if (paras.contains("xml")==false)
	{
		res["error"] = tr("xml must  exist in paraments.");
		return std::move(res);
	}
	QString name = paras["xml"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("xml could not be empty.");
		return std::move(res);
	}

	bool ok = this->xml_save(name);
	res ["return"] = ok;
	return res;
}
/**
 * @brief func_save_xml is a internal function for plugin call_func "save_xml"
 *
 * the paraments used by paras is listed below.
 * function=load_xml;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_load_xml		(const QMap<QString, QVariant> & paras)
{
	//!xml: user should specify xml filename or the function calll will fail;
	QMap<QString, QVariant> res;
	if (paras.contains("xml")==false)
	{
		res["error"] = tr("xml must  exist in paraments.");
		return std::move(res);
	}
	QString name = paras["xml"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("xml could not be empty.");
		return std::move(res);
	}

	bool ok = this->xml_load(name);
	res ["return"] = ok;
	return res;
}
/**
 * @brief func_add_reource is a internal function for plugin call_func "add_reource"
 *
 * the paraments used by paras is listed below.
 * function=add_reource;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_add_reource	(const QMap<QString, QVariant> & paras)
{
	//!xml: user should specify xml filename or the function calll will fail;
	QMap<QString, QVariant> res;
	if (paras.contains("name")==false || paras.contains("filename")==false || paras.contains("centerx")==false || paras.contains("centery")==false)
	{
		res["error"] = tr("name, filename, centerx,centery must  exist in paraments.");
		return std::move(res);
	}
	QString name = paras["name"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("name could not be empty.");
		return std::move(res);
	}
	QString filename = paras["filename"].toString();
	if (filename.size()==0)
	{
		res["error"] = tr("filename could not be empty.");
		return std::move(res);
	}
	int cenx = paras["centerx"].toInt();
	int ceny = paras["centery"].toInt();
	QTVP_GEOMARKER::tag_icon icon;
	icon.centerx = cenx;
	icon.centery = ceny;
	icon.filename = filename;
	icon.name =name;
	bool ok = icon.icon.load(icon.filename);
	res ["return"] =ok ;
	if (ok)
	{
		m_map_icons[icon.name] = icon;
		refreshIconModel();
	}

	return std::move(res);

}
/**
 * @brief func_save_reources is a internal function for plugin call_func "save_reources"
 *
 * the paraments used by paras is listed below.
 * function=save_reources;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_save_reources	(const QMap<QString, QVariant> & paras)
{
	//!xml: user should specify xml filename or the function calll will fail;
	QMap<QString, QVariant> res;
	if (paras.contains("xml")==false)
	{
		res["error"] = tr("xml must  exist in paraments.");
		return std::move(res);
	}
	QString name = paras["xml"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("xml could not be empty.");
		return std::move(res);
	}

	bool ok = this->xml_icon_save(name);
	res ["return"] = ok;
	return res;
}
/**
 * @brief func_load_reources is a internal function for plugin call_func "load_reources"
 *
 * the paraments used by paras is listed below.
 * function=save_reources;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_load_reources	(const QMap<QString, QVariant> & paras)
{
	//!xml: user should specify xml filename or the function call will fail;
	QMap<QString, QVariant> res;
	if (paras.contains("xml")==false)
	{
		res["error"] = tr("xml must  exist in paraments.");
		return std::move(res);
	}
	QString name = paras["xml"].toString();
	if (name.size()==0)
	{
		res["error"] = tr("xml could not be empty.");
		return std::move(res);
	}

	bool ok = this->xml_icon_load(name);
	res ["return"] = ok;
	return res;
}
