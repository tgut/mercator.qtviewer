#include "qtvplugin_geomarker.h"
#include "ui_qtvplugin_geomarker.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "geographicsellipseitem.h"
#include "geographicslineitem.h"
#include "geographicspolygonitem.h"
#include "geographicsrectitem.h"
#include <QDebug>
#include <QXmlStreamAttributes>
#include <QMap>
/**
 * geomarker supports these function calls.
 *
 *
 *	ATTENTION!THIS FUNCTION IS NOT THREAD SAFE! Calls should be made only in main thread, also called "UI Thread"
 * @param paras	the key-value style paraments.
 * @return QMap<QString, QVariant>	the key-value style return values.
 */
QMap<QString, QVariant> qtvplugin_geomarker::call_func(const  QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	if (paras.contains("function"))
	{
		QString funct = paras["function"].toString();
		if (funct=="update_point")
		{
			update_point(paras);
			scheduleRefreshMarks();
			scheduleUpdateMap();
		}
		else if (funct=="update_line")
		{
			update_line(paras);
			scheduleRefreshMarks();
			scheduleUpdateMap();

		}
		else if (funct=="update_polygon")
		{
			update_polygon(paras);
			scheduleRefreshMarks();
			scheduleUpdateMap();

		}
		else if (funct=="update_prop")
		{

		}
		else
			res["error"] = QString("unknown function \"%1\".").arg(funct);
	}
	else
		res["error"] = "\"function\" keyword not specified, nothing to do.";
	return std::move(res);
}
bool qtvplugin_geomarker::saveToXml(QString xml)
{
	QFile fp(xml);
	if (fp.open(QIODevice::WriteOnly)==false)
		return false;

	QXmlStreamWriter stream(&fp);
	stream.setAutoFormatting(true);

	//0. Start Document
	stream.writeStartDocument();

	stream.writeStartElement("geomarker");
	stream.writeAttribute("version","1.0");

	//1. for each mark, write a root element
	QList<QTVP_GEOMARKER::geoItemBase *> items = m_pScene->geo_items();
	foreach (QTVP_GEOMARKER::geoItemBase * item, items)
	{
		QTVP_GEOMARKER::geo_item_type x_tp = item->item_type();
		QString x_name = item->item_name();

		//1.1. Mark
		stream.writeStartElement("mark");
		stream.writeAttribute("name",x_name);
		stream.writeAttribute("type",QString("%1").arg((int)x_tp));
		switch (x_tp)
		{
		case QTVP_GEOMARKER::ITEAMTYPE_RECT_POINT:
		{
			QTVP_GEOMARKER::geoGraphicsRectItem * pU = dynamic_cast<QTVP_GEOMARKER::geoGraphicsRectItem *>(item);
			if (pU)
			{
				//1.2. geo
				stream.writeStartElement("geo");
				stream.writeAttribute("coords","1");
				stream.writeTextElement("cod",QString("%1,%2").arg(pU->lat(),0,'f',7).arg(pU->lon(),0,'f',7));
				stream.writeEndElement(); // geo
				//1.2 style
				stream.writeStartElement("style");
				stream.writeTextElement("width",QString("%1").arg(pU->width()));
				stream.writeTextElement("height",QString("%1").arg(pU->height()));
				stream.writeTextElement("color_pen",color2string(pU->pen().color()));
				stream.writeTextElement("style_pen",QString("%1").arg(int(pU->pen().style())));
				stream.writeTextElement("width_pen",QString("%1").arg(int(pU->pen().width())));
				stream.writeTextElement("color_brush",color2string(pU->brush().color()));
				stream.writeTextElement("style_brush",QString("%1").arg(int(pU->brush().style())));

			}
		}
			break;
		case QTVP_GEOMARKER::ITEAMTYPE_ELLIPSE_POINT:
		{
			QTVP_GEOMARKER::geoGraphicsEllipseItem * pU = dynamic_cast<QTVP_GEOMARKER::geoGraphicsEllipseItem *>(item);
			if (pU)
			{
				//1.2. geo
				stream.writeStartElement("geo");
				stream.writeAttribute("coords","1");
				stream.writeTextElement("cod",QString("%1,%2").arg(pU->lat(),0,'f',7).arg(pU->lon(),0,'f',7));
				stream.writeEndElement(); // geo
				//1.2 style
				stream.writeStartElement("style");
				stream.writeTextElement("width",QString("%1").arg(pU->width()));
				stream.writeTextElement("height",QString("%1").arg(pU->height()));
				stream.writeTextElement("color_pen",color2string(pU->pen().color()));
				stream.writeTextElement("style_pen",QString("%1").arg(int(pU->pen().style())));
				stream.writeTextElement("width_pen",QString("%1").arg(int(pU->pen().width())));
				stream.writeTextElement("color_brush",color2string(pU->brush().color()));
				stream.writeTextElement("style_brush",QString("%1").arg(int(pU->brush().style())));

			}
		}
			break;
		case QTVP_GEOMARKER::ITEAMTYPE_LINE:
		{
			QTVP_GEOMARKER::geoGraphicsLineItem * pU = dynamic_cast<QTVP_GEOMARKER::geoGraphicsLineItem *>(item);
			if (pU)
			{
				//1.2. geo
				stream.writeStartElement("geo");
				stream.writeAttribute("coords","2");
				stream.writeTextElement("cod",QString("%1,%2").arg(pU->lat1(),0,'f',7).arg(pU->lon1(),0,'f',7));
				stream.writeTextElement("cod",QString("%1,%2").arg(pU->lat2(),0,'f',7).arg(pU->lon2(),0,'f',7));
				stream.writeEndElement(); // geo
				//1.2 style
				stream.writeStartElement("style");
				stream.writeTextElement("color_pen",color2string(pU->pen().color()));
				stream.writeTextElement("style_pen",QString("%1").arg(int(pU->pen().style())));
				stream.writeTextElement("width_pen",QString("%1").arg(int(pU->pen().width())));

			}
		}
			break;
		case QTVP_GEOMARKER::ITEAMTYPE_POLYGON:
		{
			QTVP_GEOMARKER::geoGraphicsPolygonItem * pU = dynamic_cast<QTVP_GEOMARKER::geoGraphicsPolygonItem *>(item);
			if (pU)
			{
				//1.2. geo
				stream.writeStartElement("geo");
				QPolygonF pl = pU->llas();
				int nPl = pl.size();
				stream.writeAttribute("coords",QString("%1").arg(nPl));
				foreach (QPointF pf, pl)
					stream.writeTextElement("cod",QString("%1,%2").arg(pf.y(),0,'f',7).arg(pf.x(),0,'f',7));
				stream.writeEndElement(); // geo
				//1.2 style
				stream.writeStartElement("style");
				stream.writeTextElement("color_pen",color2string(pU->pen().color()));
				stream.writeTextElement("style_pen",QString("%1").arg(int(pU->pen().style())));
				stream.writeTextElement("width_pen",QString("%1").arg(int(pU->pen().width())));
				stream.writeTextElement("color_brush",color2string(pU->brush().color()));
				stream.writeTextElement("style_brush",QString("%1").arg(int(pU->brush().style())));


			}
		}
			break;
		default:
			break;
		}

		QColor colorText = item->labelColor();
		stream.writeTextElement("color_label",color2string(colorText));

		int fsize = item->labelFont().pointSize();
		stream.writeTextElement("size_label",QString("%1").arg(fsize));

		int weight = item->labelFont().weight();
		stream.writeTextElement("weight_label",QString("%1").arg(weight));

		stream.writeEndElement(); // style

		//1.2 properties
		stream.writeStartElement("properties");
		int props = item->prop_counts();
		QStringList lstNames = item->prop_names();
		QVariantList lstValues = item->prop_values();
		for (int i=0;i<props;++i)
			stream.writeTextElement(lstNames[i],lstValues[i].toString());
		//1.2 properties
		stream.writeEndElement();
		// 1.1mark
		stream.writeEndElement();
	}
	stream.writeEndElement();
	//0. End Document
	stream.writeEndDocument();
	fp.flush();
	fp.close();
	return true;
}
QMap<QString,QString> qtvplugin_geomarker::attribs(const QXmlStreamAttributes & ats)
{
	QMap<QString, QString > attr;
	foreach(QXmlStreamAttribute a, ats)
		attr[a.name().toString()] = a.value().toString();
	return std::move(attr);
}
bool qtvplugin_geomarker::loadFromXml(QString xmlfile)
{
	bool res = true;
	QString errMessage;
	QFile fp(xmlfile);
	if (fp.open(QIODevice::ReadOnly)==false)
		return false;
	QXmlStreamReader xml(&fp);
	if (xml.readNextStartElement())
	{
		if (xml.name()=="geomarker")
		{
			QMap<QString,QString>  att_geoMarker = attribs(xml.attributes());
			if (att_geoMarker["version"].toDouble()>=1)
			{
				while (!xml.atEnd() && res)
				{
					xml.readNext();
					if (xml.tokenType()==QXmlStreamReader::StartElement)
					{
						if (xml.name()=="mark")
						{
							tag_xml_mark mark;
							res = readMark(xml,mark,errMessage);
							if (res)
								res = update_mark(mark);
						}
					}
				}
			}
			else
			{
				res = false;
				errMessage = tr("Version must >=1.0.");
			}

		}
		else
		{
			res = false;
			errMessage = tr("This XML is not a geomarker format file.");
		}
	}
	else
	{
		res = false;
		errMessage = tr("Empty XML.");
	}
	fp.close();
	if (res==false)
	{
		QMap<QString,QVariant> evt_error;
		evt_error["source"] = get_name();
		evt_error["destin"] = "ALL";
		evt_error["name"] = "error";
		evt_error["class"] = "QXmlStreamReader";
		evt_error["file"] = xmlfile;
		evt_error["detail"] = errMessage;
		m_pVi->post_event(evt_error);

	}
	return res;
}
bool  qtvplugin_geomarker::readMark(QXmlStreamReader & xml,tag_xml_mark & mark, QString & errMessage)
{
	//mark
	QMap<QString,QString >  att_marker = attribs(xml.attributes());
	mark.name	= att_marker["name"];
	mark.type	= att_marker["type"].toInt();
	if (mark.name.size()==0 || mark.type < 1)
	{
		errMessage = tr("mark name is null or type error .");
		return false;
	}

	while (xml.readNextStartElement())
	{
		if (xml.name()=="geo")
		{
			if (false==readGeo(xml,mark,errMessage))
				return false;
		}
		else if (xml.name()=="style")
		{
			if (false==readStyle(xml,mark,errMessage))
				return false;
		}
		else if (xml.name()=="properties")
		{
			if (false==readProps(xml,mark,errMessage))
				return false;
		}
		else
			;
	}



	return true;
}
bool qtvplugin_geomarker::readGeo(QXmlStreamReader & xml,tag_xml_mark & mark, QString & errMessage)
{
	QMap<QString,QString >  att_geo = attribs(xml.attributes());
	int coords = att_geo["coords"].toInt();
	if (coords<1)
	{
		errMessage = tr("coords is 0 .");
		return false;
	}
	for (int i=0;i<coords; ++i)
	{
		QPointF curCd;
		if (false ==xml.readNextStartElement())
		{
			errMessage = tr("missing geo coords .");
			return false;
		}
		QString txtCod = xml.readElementText();
		QStringList lst = txtCod.split(",");
		curCd.setY(lst.first().toFloat());
		curCd.setX(lst.last().toFloat());
		mark.geoPoints.push_back(curCd);
	}
	xml.skipCurrentElement();

	return true;

}

bool qtvplugin_geomarker::readStyle(QXmlStreamReader & xml,tag_xml_mark & mark, QString & /*errMessage*/)
{
	do
	{
		if (false==xml.readNextStartElement())
			break;
		else
		{
			QString prop_name = xml.name().toString();
			QString prop_v = xml.readElementText();
			mark.styles[prop_name] = prop_v;
		}
	}while(xml.error()==QXmlStreamReader::NoError);
	return true;
}

bool qtvplugin_geomarker::readProps(QXmlStreamReader & xml,tag_xml_mark & mark, QString & /*errMessage*/)
{
	do
	{
		if (false==xml.readNextStartElement())
			break;
		else
		{
			QString prop_name = xml.name().toString();
			QString prop_v = xml.readElementText();
			mark.props[prop_name] = prop_v;
		}
	}while(xml.error()==QXmlStreamReader::NoError);
	return true;
}
bool qtvplugin_geomarker::update_mark(tag_xml_mark & mark)
{
	if (mark.geoPoints.size()==0)
		return false;
	QString name = mark.name;
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

	int ptdd = mark.styles["style_pen"].toInt();
	if (ptdd < 0 || ptdd >=7)
		ptdd = 1;
	QColor penColor = string2color( mark.styles["color_pen"]);
	int penWidth = mark.styles["width_pen"].toInt();
	if (penWidth<0)	penWidth = 1;
	QPen pen;//(QBrush(color),width,pst[ptdd]);
	pen.setColor(penColor);
	pen.setWidth(penWidth);
	pen.setStyle(pst[ptdd]);

	int btdd = mark.styles["style_brush"].toInt();
	if (btdd < 0 || btdd >=15)
		btdd = 1;
	QColor brushColor = string2color( mark.styles["color_brush"]);
	QBrush brush;
	brush.setColor(brushColor);
	brush.setStyle(bst[btdd]);

	QTVP_GEOMARKER::geoItemBase * newitem = 0;

	if (mark.type==QTVP_GEOMARKER::ITEAMTYPE_RECT_POINT || mark.type==QTVP_GEOMARKER::ITEAMTYPE_ELLIPSE_POINT)
	{
		double lat = mark.geoPoints.first().y();
		double lon = mark.geoPoints.first().x();
		int width = mark.styles["width"].toInt();
		if (width ==0)	width = 8;
		int height = mark.styles["height"].toInt();
		if (height ==0)	height = 8;
		if (mark.type==QTVP_GEOMARKER::ITEAMTYPE_RECT_POINT)
			newitem = update_point<QTVP_GEOMARKER::geoGraphicsRectItem>(name,lat,lon,width,height,pen,brush);
		else
			newitem = update_point<QTVP_GEOMARKER::geoGraphicsEllipseItem>(name,lat,lon,width,height,pen,brush);
	}
	else if (mark.type==QTVP_GEOMARKER::ITEAMTYPE_LINE)
	{
		if ( mark.geoPoints.size()<2)
			return false;
		double lat1 = mark.geoPoints.first().y();
		double lon1 = mark.geoPoints.first().x();
		double lat2 = mark.geoPoints.last().y();
		double lon2 = mark.geoPoints.last().x();
		newitem = update_line(name,lat1,lon1,lat2,lon2,pen);
	}
	else if (mark.type==QTVP_GEOMARKER::ITEAMTYPE_POLYGON)
	{
		newitem = update_polygon(name,mark.geoPoints,pen,brush);
	}
	else
		return false;
	if (newitem)
	{
		int fontSz = mark.styles["size_label"].toInt();
		if (fontSz==0)	fontSz = 9;
		int fontWeight = mark.styles["weight_label"].toInt();
		QColor textColor = string2color( mark.styles["color_label"]);

		QFont f = newitem->labelFont();
		f.setPointSize(fontSz);
		f.setWeight(fontWeight);
		newitem->setLabelFont(f);
		newitem->setLabelColor(textColor);

		for (QMap<QString,QString>::iterator p = mark.props.begin();p != mark.props.end();++p )
		{
			newitem->set_prop_data(p.key(),p.value());
		}

	}

	return true;
}

QTVP_GEOMARKER::geoItemBase * qtvplugin_geomarker::update_point		(const QMap<QString, QVariant> & paras)
{
	QString name = paras["name"].toString();
	if (name.size()==0)
		return 0;
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

	return newitem;
}
QTVP_GEOMARKER::geoItemBase * qtvplugin_geomarker::update_line		(const QMap<QString, QVariant> & paras)
{
	QString name = paras["name"].toString();
	if (name.size()==0)
		return 0;
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

	double lat1 =paras["lat1"].toDouble();
	double lon1 = paras["lon1"].toDouble();
	double lat2 =paras["lat2"].toDouble();
	double lon2 = paras["lon2"].toDouble();

	newitem = update_line(name,lat1,lon1,lat2,lon2,pen);
	return newitem;
}
QTVP_GEOMARKER::geoItemBase * qtvplugin_geomarker::update_polygon		(const QMap<QString, QVariant> & paras)
{
	QString name = paras["name"].toString();
	if (name.size()==0)
		return 0;
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
	}
	if (pl.size()>2)
		newitem = update_polygon(name,pl,pen,brush);
	return newitem;
}
