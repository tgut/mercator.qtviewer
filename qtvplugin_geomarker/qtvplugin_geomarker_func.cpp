#include "qtvplugin_geomarker.h"
#include "ui_qtvplugin_geomarker.h"
#include "geographicsellipseitem.h"
#include "geographicslineitem.h"
#include "geographicspolygonitem.h"
#include "geographicsrectitem.h"
#include "geographicsmultilineitem.h"
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
 * 12.add_resource		add an resource (eg, icon) to current resource list. icon mark can reference icons in current resource list.
 * 13 save_resources		save current resource list to disk files.
 * 14 load_resources		load previewsly saved list file from disk to current resource list.
 * 15 load_xml			load xml from diskette
 * 16 sava_xml			save marks to diskette in xml format
 * 17 props_vis(ibiliuty)	collapse detail display for items.
 * 18 show_props		show_details
 * 19 set_mod			set current mod.0=display, 1 = rect selection
 * 20 selection_clear	clear all selected items' status.
 * 21 selection_delete	delete all selected items.
 * 22 selected_items	return all selected item names
 * 23 set_default_style	will set default style for item mark
 * 24 default_style		will return current default style
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
	m_map_pluginFunctions["add_resource"]	= std::bind(&qtvplugin_geomarker::func_add_resource,	this,std::placeholders::_1);
	m_map_pluginFunctions["save_resources"]	= std::bind(&qtvplugin_geomarker::func_save_resources,	this,std::placeholders::_1);
	m_map_pluginFunctions["load_resources"]	= std::bind(&qtvplugin_geomarker::func_load_resources,	this,std::placeholders::_1);
	m_map_pluginFunctions["props_vis"]		= std::bind(&qtvplugin_geomarker::func_props_vis,		this,std::placeholders::_1);
	m_map_pluginFunctions["show_props"]		= std::bind(&qtvplugin_geomarker::func_show_props,		this,std::placeholders::_1);
	m_map_pluginFunctions["set_mod"]		= std::bind(&qtvplugin_geomarker::func_set_mod,	this,std::placeholders::_1);
	m_map_pluginFunctions["selection_clear"]= std::bind(&qtvplugin_geomarker::func_selection_clear,	this,std::placeholders::_1);
	m_map_pluginFunctions["selection_delete"]=std::bind(&qtvplugin_geomarker::func_selection_delete,this,std::placeholders::_1);
	m_map_pluginFunctions["selected_items"]	= std::bind(&qtvplugin_geomarker::func_selected_items,	this,std::placeholders::_1);
	m_map_pluginFunctions["set_default_style"]=std::bind(&qtvplugin_geomarker::func_set_default_style,this,std::placeholders::_1);
	m_map_pluginFunctions["default_style"]	= std::bind(&qtvplugin_geomarker::func_default_style,	this,std::placeholders::_1);
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
	QPen pen (m_default_style.pen);
	QBrush brush (m_default_style.brush);
	qreal width = m_default_style.n_point_width;
	qreal height =  m_default_style.n_point_height;
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

	int tpn = m_default_style.n_point_rect == 0 ? 2:1;
	if (paras.contains("type"))
			tpn = paras["type"].toInt();
	if (tpn > 2 || tpn <1) tpn = 1;
	QTVP_GEOMARKER::geo_item_type tpe = static_cast< QTVP_GEOMARKER::geo_item_type > (tpn);
	//update using same function in UI
	if (tpe==QTVP_GEOMARKER::ITEAMTYPE_RECT_POINT)
		newitem = update_point<QTVP_GEOMARKER::geoGraphicsRectItem>(name,lat,lon,width,height,pen,brush);
	else
		newitem = update_point<QTVP_GEOMARKER::geoGraphicsEllipseItem>(name,lat,lon,width,height,pen,brush);
	if (newitem)
	{
		QFont f ( m_default_style.font );
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
		else
			newitem->setLabelColor(m_default_style.text_color);
		//scheduleRefreshMarks();
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
	QString icon_name = m_default_style.icon_name;
	qreal scale = m_default_style.scale;
	qreal rot = m_default_style.rotate;
	int smooth = m_default_style.smooth;
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
		QFont f (m_default_style.font);
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
		else
			newitem->setLabelColor( m_default_style.text_color);
		//scheduleRefreshMarks();
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
	QPen pen (m_default_style.pen);

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
		QFont f (m_default_style.font);
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
		else
			newitem->setLabelColor(m_default_style.text_color);
		//scheduleRefreshMarks();
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

	int type = m_default_style.multiline==0?4:6;//polygon
	if (paras.contains("type")==true)
	{
		type = paras["type"].toInt();
	}

	if (name.size()==0)
	{
		res["error"] = tr("name could not be empty.");
		return std::move(res);
	}


	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	QPen pen(m_default_style.pen);
	QBrush brush(m_default_style.brush);

	//if the mark is already exist, we will get its orgional style as default .
	if (base)
	{
		if (base->item_type()==QTVP_GEOMARKER::ITEAMTYPE_POLYGON && type !=6)
		{
			QTVP_GEOMARKER::geoGraphicsPolygonItem * it = dynamic_cast<QTVP_GEOMARKER::geoGraphicsPolygonItem * >(base);
			if (it)
			{
				pen = it->pen();
				brush = it->brush();
			}
		}
		else if (base->item_type()==QTVP_GEOMARKER::ITEAMTYPE_MULTILINE && type !=4)
		{
			QTVP_GEOMARKER::geoGraphicsMultilineItem * it = dynamic_cast<QTVP_GEOMARKER::geoGraphicsMultilineItem * >(base);
			if (it)
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
	if (pl.size()>2 || (type!=4 && pl.size()>1))
		newitem = update_polygon(name,pl,pen,brush,type==6?true:false);
	else
		res["error"] = tr("polygon must contain at least 3 points,lat0,lat1,lat2 and lon0,lon1, lon2.");

	if (newitem)
	{
		QFont f (m_default_style.font);
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
		else
			newitem->setLabelColor(m_default_style.text_color);
		//scheduleRefreshMarks();
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
		if (base->props_visible())
		{
			base->show_props(false);
			base->show_props(true);
		}
		//scheduleRefreshMarks();
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
		needUpdate = true;
		scheduleRefreshMarks();
	}
	if (needUpdate)
	{
		//scheduleRefreshMarks();
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
		if (base->props_visible())
		{
			base->show_props(false);
			base->show_props(true);
		}
		//scheduleRefreshMarks();
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
		case QTVP_GEOMARKER::ITEAMTYPE_MULTILINE:
		{
			QTVP_GEOMARKER::geoGraphicsMultilineItem * pU = dynamic_cast<QTVP_GEOMARKER::geoGraphicsMultilineItem *>(item);
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
	if (ok)
	{
		//scheduleRefreshMarks();
		scheduleUpdateMap();
	}
	res ["return"] = ok;
	return res;
}
/**
 * @brief func_add_resource is a internal function for plugin call_func "add_resource"
 *
 * the paraments used by paras is listed below.
 * function=add_resource;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_add_resource	(const QMap<QString, QVariant> & paras)
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
 * @brief func_save_resources is a internal function for plugin call_func "save_resources"
 *
 * the paraments used by paras is listed below.
 * function=save_resources;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_save_resources	(const QMap<QString, QVariant> & paras)
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
 * @brief func_load_resources is a internal function for plugin call_func "load_resources"
 *
 * the paraments used by paras is listed below.
 * function=save_resources;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_load_resources	(const QMap<QString, QVariant> & paras)
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
/**
 * @brief func_props_vis is a internal function for plugin call_func "props_vis"
 *
 * the paraments used by paras is listed below.
 * function=props_vis;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_props_vis		(const QMap<QString, QVariant> & paras)
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
	if (set_names.size())
	{
		ct = 0;
		foreach (QString key,set_names)
		{
			QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(key);
			if (base)
			{
				bool vis = base->props_visible();
				if (vis)
					res[key] = 1;
				else
					res[key] = 0;
			}
			else
				res[key] = 0;
		}
	}
	else
	{
		QList< QTVP_GEOMARKER::geoItemBase *  > lst = m_pScene->geo_items();
		foreach (QTVP_GEOMARKER::geoItemBase * key,lst)
		{
			bool vis = key->props_visible();
			if (vis)
				res[key->item_name()] = 1;
			else
				res[key->item_name()] = 0;
		}
	}

	//! the return value is "name=v;" serials, v=0 means props are collapsed, otherwise means props is visible.
	return std::move(res);
}
/**
 * @brief func_show_props is a internal function for plugin call_func "show_props"
 *
 * the paraments used by paras is listed below.
 * function=show_props;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_show_props		(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	//! mark props visibility should be stored in paire,
	//! for example, StarBar=0;ruijing Hospital=1;shunfun express=1;
	//! all other props will be hidden.
	bool needupdate = false;

	QList< QTVP_GEOMARKER::geoItemBase *  > lst = m_pScene->geo_items();
	foreach (QTVP_GEOMARKER::geoItemBase * base,lst)
	{
		QString key = base->item_name();
		if (paras.contains(key))
		{
			bool vis = paras[key].toInt()==0?false:true;
			bool bOldVis = base->props_visible();
			if (vis!=bOldVis)
			{
				needupdate = true;
				base->show_props(vis);
				bOldVis = vis;
			}
			res[key] = bOldVis?1:0;
		}
		else
		{
			bool bOldVis = base->props_visible();
			if (bOldVis)
				needupdate = true;
			base->show_props(false);
		}
	}
	if (needupdate)
	{
		//scheduleRefreshMarks();
		scheduleUpdateMap();
	}
	//! the return value is "name=v;" serials, v=0 means props are collapsed, otherwise means props is visible.
	return std::move(res);
}
//tools methods
/**
 * @brief func_set_mod is a internal function for plugin call_func "set_mod"
 *
 * the paraments used by paras is listed below.
 * function=set_mod;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_set_mod		(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	//! the current mod can be set using the UI tools "radio button".
	//!
	//! all other props will be hidden.
	if (paras.contains("mod")==false)
	{
		res["error"] = tr("mod must  exist in paraments.");
		return std::move(res);
	}
	int mod = paras["mod"].toInt();
	if (mod==0)
	{
		ui->radioButton_display->setChecked(true);
		ui->toolBox_marks->setCurrentIndex(0);
		m_sel_ptStart_World = m_sel_ptEnd_World = QPointF();
		m_currentTools = qtvplugin_geomarker::TOOLS_DISPLAY_ONLY;
		layer_interface * pOSM =  m_pVi->layer("OSM");
		if (pOSM)
		{
			pOSM->set_active(true);
			m_pVi->adjust_layers(pOSM);
		}
		m_pVi->UpdateWindow();
		m_pVi->updateLayerGridView();
	}
	else if (mod==1)
	{
		ui->radioButton_rect_selection->setChecked(true);
		ui->toolBox_marks->setCurrentIndex(1);
		m_currentTools = qtvplugin_geomarker::TOOLS_RECT_SELECTION;
		m_sel_ptStart_World = m_sel_ptEnd_World = QPointF();
		m_pVi->adjust_layers(this);
		m_pVi->UpdateWindow();
		m_pVi->updateLayerGridView();
	}
	else
	{
		res["error"] = tr("mod is not valid in paraments. 0=display, 1=rect selection");
		return std::move(res);
	}

	return std::move(res);
}

//selection methods
/**
 * @brief func_set_mod is a internal function for plugin call_func "selection_clear"
 *
 * the paraments used by paras is listed below.
 * function=selection_clear;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_selection_clear(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	this->on_pushButton_sel_clear_clicked();
	return std::move(res);
}
/**
 * @brief func_set_mod is a internal function for plugin call_func "selection_delete"
 *
 * the paraments used by paras is listed below.
 * function=selection_delete;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_selection_delete(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	this->on_pushButton_sel_delselected_clicked();
	return std::move(res);
}
/**
 * @brief func_set_mod is a internal function for plugin call_func "selected_items"
 *
 * the paraments used by paras is listed below.
 * function=selected_items;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 * the return value is stored in pairs, name0=??;name1=??;name2=??...namen-1=??...;
 */
QMap<QString, QVariant>			qtvplugin_geomarker::func_selected_items	(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	int ct = 0;
	foreach (QString namestr,m_set_itemNameSelected)
	{
		QString keystr = QString("name%1").arg(ct++);
		res[keystr] = namestr;
	}
	//! the mark names will be stored in key-value pairs, with
	//! name0=??;name1=??;name2=??...namen-1=??
	return std::move(res);
}
/**
 * @brief func_set_default_style is a internal function for plugin call_func "set_default_style"
 *
 * the paraments used by paras is listed below.
 * function=set_default_style;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 * the return value will be empty if no error happens.;
 */
QMap<QString, QVariant> qtvplugin_geomarker::func_set_default_style(const QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	QString errMsg;
	//! this function call will first load default style. then, if these paras is specified below,
	//! new  default style will be defined.
	this->style_load();
	//! style_pen from 0~6, is corresponds to the pen combo-box in UI system.
	if ( paras.contains("style_pen"))
	{
		int ptdd =paras["style_pen"].toInt();
		if (ptdd < 0 || ptdd >=7)
		{
			errMsg += "style_pen exceeds valid bound.";
			ptdd = 1;
		}
		ui->comboBox_linePad->setCurrentIndex(ptdd);
	}
	//! color_pen has 4 pen color band values splitted by comma, r,g,b,a
	if ( paras.contains("color_pen"))
	{
		ui->lineEdit_PenColor->setText(paras["color_pen"].toString());
	}
	//! width_pen has a value >0 , stand for the point width of the pen on screen.
	if ( paras.contains("width_pen"))
	{
		int penWidth =paras["width_pen"].toInt();
		if (penWidth<0)
		{
			errMsg += "width_pen must >0.";
			penWidth = 1;
		}
		ui->spinBox_penWidth->setValue(penWidth);
	}
	//! style_brush from 0~14, is corresponds to the brush style combo-box in UI system.
	if ( paras.contains("style_brush"))
	{
		int btdd = paras["style_brush"].toInt();
		if (btdd < 0 || btdd >=15)
		{
			btdd = 1;
			errMsg += "style_brush exceeds valid bounds.";
		}
		ui->comboBox_fillPad->setCurrentIndex(btdd);
	}

	//! color_brush has 4 brush color band values splitted by comma, r,g,b,a
	if ( paras.contains("color_brush"))
	{
		ui->lineEdit_FillColor->setText( paras["color_brush"].toString());
	}

	//! width has is a integer, means the default width of a point mark
	if ( paras.contains("width"))
	{
		int point_width = paras["width"].toInt();
		if (point_width==0) point_width = 8;
		ui->spinBox_point_width->setValue(point_width);
	}
	//! height has is a integer, means the default width of a point mark
	if ( paras.contains("height"))
	{
		int point_height = paras["height"].toInt();
		if (point_height==0) point_height = 8;
		ui->spinBox_point_height->setValue(point_height);
	}
	//! mark default point_type select , 1 means rect, 2 means Ecilips
	if (paras.contains("point_type"))
	{
		int tpn = paras["point_type"].toInt();
		switch (tpn)
		{
		case 1:
			ui->radioButton_PointRect->setChecked(true);
			break;
		case 2:
			ui->radioButton_PointRound->setChecked(true);
			break;
		default:
			break;
		}
	}
	//! mark default polygon_type select , 4 means polygon, 6 mean multiline
	if (paras.contains("polygon_type"))
	{
		int tpn = paras["polygon_type"].toInt();
		switch (tpn)
		{
		case 4:
			ui->checkBox_multiline->setChecked(false);
			break;
		case 6:
			ui->checkBox_multiline->setChecked(true);
			break;
		default:
			break;
		}
	}
	//! size_label stands for the text label font pixel size from 1 - 720, with a normal value 9.
	if ( paras.contains("size_label"))
	{
		int fontSz = paras["size_label"].toInt();
		if (fontSz==0)	fontSz = 9;
		ui->spinBox_fontSize->setValue(fontSz);
	}
	//! weight_label is the bolder rate for  text renderring, from 1 ~ 99, 99 is the heaviest.
	if ( paras.contains("weight_label"))
	{
		int fontWeight = paras["weight_label"].toInt();
		if (fontWeight>=0 && fontWeight <100)
			ui->spinBox_textWeight->setValue(fontWeight);
	}

	//! color_label has 4 text color band values splitted by comma, r,g,b,a
	if ( paras.contains("color_label"))
	{
		ui->lineEdit_TextColor->setText(paras["color_label"].toString());
	}
	//! icon is the name that this mark will use.
	if ( paras.contains("icon"))
	{
		QString icn = paras["icon"].toString();
		if (m_map_icons.contains(icn))
			ui->comboBox_icons->setCurrentText(icn);
		else
			errMsg += "icon is not exist";
	}
	//! scale is the zoom ratio that this icon will use, 1.0 means no zoom
	if ( paras.contains("scale"))
	{
		qreal sc = paras["scale"].toReal();
		if (sc >0 )
			ui->lineEdit_icon_scale->setText(QString("%1").arg(sc));
	}
	//! rotate is the rotate angle that this icon will use, 0.0 means no rotate
	if ( paras.contains("rotate"))
	{
		qreal rt = paras["rotate"].toReal();
		ui->lineEdit_icon_rotate->setText(QString("%1").arg(rt));
	}
	//!smooth is the transform mode that this icon will use. 0 mean not smooth, but faster. 1 mean smooth.
	if ( paras.contains("smooth"))
	{
		int smt =paras["smooth"].toInt();
		ui->checkBox_icon_smooth->setChecked(smt==0?false:true);
	}
	if (errMsg.size())
		res["warning"] = errMsg;
	//style save
	style_save();
	return std::move(res);
}
/**
 * @brief func_default_style is a internal function for plugin call_func "default_style"
 *
 * the paraments used by paras is listed below.
 * function=default_style;
 * @param paras The key-value style paraments.
 * @return QMap<QString, QVariant>  if error happens, a property called "error" will store the most possible reason.
 * the return value is listed below:
 */
QMap<QString, QVariant> qtvplugin_geomarker::func_default_style(const QMap<QString, QVariant> &)
{
	QMap<QString, QVariant> res;
	//! this function call will first load default style.
	this->style_load();
	//! style_pen from 0~6, is corresponds to the pen combo-box in UI system.
	res["style_pen"] = ui->comboBox_linePad->currentIndex();
	//! color_pen has 4 pen color band values splitted by comma, r,g,b,a
	res["color_pen"] = ui->lineEdit_PenColor->text();
	//! width_pen has a value >0 , stand for the point width of the pen on screen.
	res["width_pen"] = ui->spinBox_penWidth->value();
	//! style_brush from 0~14, is corresponds to the brush style combo-box in UI system.
	res["style_brush"] = ui->comboBox_fillPad->currentIndex();
	//! color_brush has 4 brush color band values splitted by comma, r,g,b,a
	res["color_brush"] = ui->lineEdit_FillColor->text();
	//! width has is a integer, means the default width of a point mark
	res["width"] = ui->spinBox_point_width->value();
	//! height has is a integer, means the default width of a point mark
	res["height"] = ui->spinBox_point_height->value();
	//! mark default point_type select , 1 means rect, 2 means Ecilips
	res["point_type"] = ui->radioButton_PointRect->isChecked()?1:2;
	//! mark default polygon_type select , 4 means polygon, 6 mean multiline
	res["polygon_type"] = ui->checkBox_multiline->isChecked()?6:4;
	//! size_label stands for the text label font pixel size from 1 - 720, with a normal value 9.
	res["size_label"] = ui->spinBox_fontSize->value();
	//! weight_label is the bolder rate for  text renderring, from 1 ~ 99, 99 is the heaviest.
	res["weight_label"] = ui->spinBox_textWeight->value();
	//! color_label has 4 text color band values splitted by comma, r,g,b,a
	res["color_label"] = ui->lineEdit_TextColor->text();
	//! icon is the name that this mark will use.
	res["icon"] = ui->comboBox_icons->currentText();
	//! scale is the zoom ratio that this icon will use, 1.0 means no zoom
	res["scale"] = ui->lineEdit_icon_scale->text();
	//! rotate is the rotate angle that this icon will use, 0.0 means no rotate
	res["rotate"] = ui->lineEdit_icon_rotate->text();
	//!smooth is the transform mode that this icon will use. 0 mean not smooth, but faster. 1 mean smooth.
	res["smooth"] = ui->checkBox_icon_smooth->isChecked()?-1:0;
	return std::move(res);
}

