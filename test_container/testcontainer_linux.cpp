#include "testcontainer.h"
#include "ui_testcontainer_linux.h"
#include <QDebug>
#include <QMessageBox>
testcontainer::testcontainer(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::testcontainer)
{
	ui->setupUi(this);
	QFont ft;
	ft.setPixelSize(9);
	ui->tableView_msg->setFont(ft);
	ui->tableView_msg->verticalHeader()->setDefaultSectionSize(16);
	Qt::WindowFlags flg =  this->windowFlags();
	flg |= Qt::WindowMinMaxButtonsHint;
	this->setWindowFlags(flg);

	m_pModel = new QStandardItemModel (0,2,this);
	m_pModel->setHeaderData(0,Qt::Horizontal,tr("ctrl"));
	m_pModel->setHeaderData(1,Qt::Horizontal,tr("msg"));
	ui->tableView_msg->setModel(m_pModel);

	m_nAnTimer = startTimer(150);

	confirmLayerNames();
}
void testcontainer::confirmLayerNames()
{
	//Get Total layers
	int n_num = ui->osmmap->osm_layer_get_count();

	//Get Layer names
	for (int i=0;i<n_num;++i)
	{
		QString strname = ui->osmmap->osm_layer_get_name(i);

		if (strname.indexOf("grid")>=0)
			m_str_gridLayerName = strname;
		else if (strname.indexOf("geomarker")>=0)
			m_str_markerLayerName = strname;
	}

}

testcontainer::~testcontainer()
{
	delete ui;
}
void testcontainer::closeEvent(QCloseEvent *)
{
	QCoreApplication::quit();
}
void testcontainer::show_message(QString message)
{
	QList<QStandardItem *> list_newrow;
	list_newrow << new QStandardItem(QString("map"));
	list_newrow << new QStandardItem(QString("%1").arg(message));
	m_pModel->appendRow(list_newrow);
	while (m_pModel->rowCount()>1024)
		m_pModel->removeRow(0);
	ui->tableView_msg->scrollToBottom();
}

void testcontainer::on_pushButton_test_adds_clicked()
{
	//get raw address
	QString str_addr = ui->osmmap->osm_get_remote_address("OSM");
	QMessageBox::information(this,"get osm address from LAYER \"OSM\"",str_addr);

	//set address to another value
	ui->osmmap->osm_set_remote_address("OSM","http://localhost/osmtile.cgi?level=%1&x=%2&y=%3");

	//get again
	QString str_addr2 = ui->osmmap->osm_get_remote_address("OSM");
	QMessageBox::information(this,"get osm address from LAYER \"OSM\"",str_addr2);

	//set address to another value
	ui->osmmap->osm_set_remote_address("OSM", str_addr);

	//get again
	QString str_addr3 = ui->osmmap->osm_get_remote_address("OSM");
	QMessageBox::information(this,"get osm address from LAYER \"OSM\"",str_addr3);

}
void testcontainer::on_pushButton_test_autodl_clicked()
{
	//get auto download flag
	int n_status = ui->osmmap->osm_get_auto_download("OSM");
	QMessageBox::information(this,"get auto download from LAYER \"OSM\"",QString("status = %1").arg(n_status) );

	//set flag to opposite option
	ui->osmmap->osm_set_auto_download(
				"OSM" ,
				n_status==0?-1:0);

	//get again
	n_status = ui->osmmap->osm_get_auto_download("OSM");
	QMessageBox::information(this,"get auto download from LAYER \"OSM\"",QString("status = %1").arg(n_status) );
}
void testcontainer::on_pushButton_test_navigate_clicked()
{
	//Get curent Level
	int n_level = ui->osmmap->osm_get_level();
	QMessageBox::information(this,"get_level",QString("level = %1").arg(n_level) );

	//Set level to a new value
	ui->osmmap->osm_set_level(n_level<5?++n_level:--n_level);

	//Let's see level again
	n_level = ui->osmmap->osm_get_level();
	QMessageBox::information(this,"get_level",QString("level = %1").arg(n_level) );

	//Get map center position
	double lat = ui->osmmap->osm_get_center_lat();
	double lon = ui->osmmap->osm_get_center_lon();
	QMessageBox::information(this,"osm_get_center_latlon",QString("lat = %1, lon=%2").arg(lat).arg(lon) );

	//Set a new center position
	lat = rand()%1700/10.0-85; lon = rand()%3600/10.0-180;
	ui->osmmap->osm_set_center_pos(lat,lon);

	//Get map center position
	lat = ui->osmmap->osm_get_center_lat();
	lon = ui->osmmap->osm_get_center_lon();
	QMessageBox::information(this,"osm_get_center_latlon",QString("lat = %1, lon=%2").arg(lat).arg(lon) );
}
void testcontainer::on_pushButton_test_layers_clicked()
{
	//Get Total layers
	int n_num = ui->osmmap->osm_layer_get_count();

	//Get Layer names
	QString strLayerNames;
	for (int i=0;i<n_num;++i)
	{
		QString strname = ui->osmmap->osm_layer_get_name(i);
		strLayerNames += strname + ";\n";
	}
	QMessageBox::information(this,"layer count",QString("osm_layer_get_count() returns  %1\n%2").arg(n_num).arg(strLayerNames) );

}
void testcontainer::on_pushButton_test_layer_move_clicked()
{
	//Move layers up and down
	ui->osmmap->osm_layer_move_up(m_str_gridLayerName);
	on_pushButton_test_layers_clicked();
	ui->osmmap->osm_layer_move_bottom(m_str_gridLayerName);
	on_pushButton_test_layers_clicked();
	ui->osmmap->osm_layer_move_top(m_str_gridLayerName);
	on_pushButton_test_layers_clicked();
	ui->osmmap->osm_layer_move_down(m_str_gridLayerName);
	on_pushButton_test_layers_clicked();

	//Set layer's visiblity
	int bv = ui->osmmap->osm_layer_get_visiable(m_str_gridLayerName);
	QMessageBox::information(this,"visibility",QString("osm_layer_get_visiable(\"grid\") returns  %1").arg(bv));
	bv = ui->osmmap->osm_layer_set_visiable(m_str_gridLayerName, bv==0?-1:0);
	QMessageBox::information(this,"visibility",QString("osm_layer_set_visiable(\"grid\") returns  %1").arg(bv));
	bv = ui->osmmap->osm_layer_set_visiable(m_str_gridLayerName, bv==0?-1:0);
	QMessageBox::information(this,"visibility",QString("osm_layer_set_visiable(\"grid\") returns  %1").arg(bv));

	//Set layer's activity
	int av = ui->osmmap->osm_layer_get_active("OSM");
	QMessageBox::information(this,"active",QString("osm_layer_get_active(\"OSM\") returns  %1").arg(av));
	av = ui->osmmap->osm_layer_set_active("OSM",av==0?-1:0);
	QMessageBox::information(this,"active",QString("osm_layer_set_active(\"OSM\") returns  %1").arg(av));
	av = ui->osmmap->osm_layer_set_active("OSM",av==0?-1:0);
	QMessageBox::information(this,"active",QString("osm_layer_set_active(\"OSM\") returns  %1").arg(av));
}
QString testcontainer::map_to_string(const QMap<QString, QVariant> & m)
{
	QString s;
	for(QMap<QString, QVariant>::const_iterator p = m.begin();p!=m.end();++p)
	{
		s += p.key();
		s += "=";
		s += p.value().toString();
		s += ";";
	}
	return std::move(s);
}

QMap<QString, QVariant> testcontainer::string_to_map(const QString & s)
{
	QMap<QString, QVariant> res;
	QStringList lst = s.split(";");
	foreach (QString s, lst)
	{
		int t = s.indexOf("=");
		if (t>0 && t< s.size())
		{
			QString name = s.left(t).trimmed();
			QString value = s.mid(t+1).trimmed();
			res[name] = value;
		}
	}
	return std::move(res);
}
void testcontainer::on_pushButton_test_grid_enable_clicked()
{
	//Get the grid plugin's ruler status
	QString res = ui->osmmap->
				osm_layer_call_function(
				m_str_gridLayerName,
				"function=get_ruler_status;");
	QMessageBox::information(this,"grid::get_ruler_status",res);

	//Check result
	QMap<QString, QVariant> mres = string_to_map(res);
	if (mres["status"].toInt())
	{
		res = ui->osmmap->
					osm_layer_call_function(
					m_str_gridLayerName,
					"function=set_ruler_status;status=0;");
		QMessageBox::information(this,"grid::set_ruler_status to false, you can call get_polygon to get polygon strings..",res);
	}
	else
	{
		res = ui->osmmap->osm_layer_call_function(
											 m_str_gridLayerName,
											 "function=set_ruler_status;status=-1;");
		QMessageBox::information(this,"grid::set_ruler_status to true, you can draw polygons on map using mouse lbutton for begin and rbutton for end.",res);
	}

}
void testcontainer::on_pushButton_test_grid_getPolygon_clicked()
{
	//Get current ploygon lla.
	QString res = ui->osmmap->
				osm_layer_call_function(
				m_str_gridLayerName,
				"function=get_polygon;");
	res.replace(";",";\n");
	res.replace("=","=\t");
	QMessageBox::information(this,"grid::get_polygon",res);

}
void testcontainer::on_pushButton_test_mark_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=delete_marks;name0=ID3;name1=ID4;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::delete_marks",res);

	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
										 QString("function=update_point;name=ID1;type=1;"
												 "lat=%1;lon=%2;"
												 "style_pen=2;color_pen=0,0,255,128;width_pen=3;"
												 "style_brush=1;color_brush=0,255,0,128;"
												 "color_label=0,0,255,96;weight_label=99;size_label=12;"
												 "width=16;height=20;")
										 .arg(rand()%1700/10.0-85)
										 .arg(rand()%3600/10.0-180)
										 );
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_point",res);


	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=update_props;name=ID1;"
												 "LABEL=Shanghai;EXPRESS=Shunfeng;Pero=IMMEDIATE;"
												 "CheckTime=2014-12-31 23:11:27;"
												 "From=Shanghai;To=Beijing");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_props",res);


	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=update_point;name=ID2;type=2;"
												 "lat=40.3737;lon=111.34347;"
												 "style_pen=3;color_pen=0,255,0,128;"
												 "width_pen=2;style_brush=3;color_brush=255,128,0,128;"
												 "width=12;height=12;");

	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_point",res);

	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=update_props;name=ID2;"
												 "LABEL=Neimeng;EXPRESS=YunDa;Pero=NORMAL;"
												 "CheckTime=2014-12-30 07:18:32;"
												 "From=Huhehaote;To=YinChuan");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_props",res);

	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=update_icon;name=ID7;"
												 "lat=1.233;lon=2.28373;"
												 "scale=2;rotate=0;smooth=1;"
												 "color_label=255,0,0,128;weight_label=99;size_label=9;"
												 "icon=default;");

	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_icon",res);

	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=update_props;name=ID7;"
												 "LABEL=COSCO;EXPRESS=TianTian;Pero=IMMD;"
												 "CheckTime=2012-12-30 07:18:32;"
												 "From=PEKING;To=LONDON;CAR=Apple Watch;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_props",res);

	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=show_props;ID7=1;ID1=0");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_props",res);
}
void testcontainer::on_pushButton_test_line_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=delete_marks;name0=ID1;name1=ID2;name2=ID4;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::delete_marks",res);

	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
										 QString("function=update_line;name=ID3;type=3;"
												 "lat0=%1;lon0=%2;"
												 "lat1=%3;lon1=%4;"
												 "style_pen=4;color_pen=255,0,0,96;"
												 "width_pen=2;")
										 .arg(rand()%1700/10.0-85)
										 .arg(rand()%3600/10.0-180)
										 .arg(rand()%1700/10.0-85)
										 .arg(rand()%3600/10.0-180)
										 );
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_line",res);


	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=update_props;name=ID3;"
												 "LABEL=HighWay;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_point",res);
}

void testcontainer::on_pushButton_test_polygon_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=delete_marks;name0=ID1;name1=ID2;name2=ID3;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::delete_marks",res);

	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=update_polygon;name=ID4;type=4;"
												 "lat0=12.2;lon0=67.3;"
												 "lat1=14.3;lon1=62.8;"
												 "lat2=22.7;lon2=66.5;"
												 "lat3=11.5;lon3=72.2;"
												 "lat4=10.8;lon4=69.4;"
												 "style_pen=2;color_pen=0,0,255,128;"
												 "width_pen=3;style_brush=1;color_brush=0,255,0,128;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_polygon",res);

	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=update_props;name=ID4;"
												 "LABEL=Region;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_point",res);

	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=update_polygon;name=ID40;type=6;"
												 "lat0=42.2;lon0=-67.3;"
												 "lat1=34.3;lon1=-62.8;"
												 "lat2=22.7;lon2=-66.5;"
												 "lat3=11.5;lon3=-72.2;"
												 "lat4=0.8;lon4=-69.4;"
												 "style_pen=2;color_pen=0,0,255,128;"
												 "width_pen=3;style_brush=1;color_brush=0,255,0,128;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_polygon",res);

	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=update_props;name=ID40;"
												 "LABEL=Multiline;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::update_point",res);

}

void testcontainer::timerEvent(QTimerEvent * e)
{
	if (e->timerId()==m_nAnTimer)
	{
		QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
													 "function=exists;name=ID7;");
		QMap<QString, QVariant> mres = string_to_map(res);
		if (mres["return"].toInt())
		{
			//Get info of this mark
			res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=mark;name=ID7");
			QMap<QString, QVariant> mparas = string_to_map(res);
			double lat = mparas["lat"].toDouble() + 0.173245467333;
			double lon = mparas["lon"].toDouble() + 0.245546767673;
			qreal rot = mparas["rotate"].toReal()+1.38745738457;
			if (rot>360) rot = 0;
			if (lat >=85) lat = -85;
			if (lon >=180) lon = -180;
			ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
										   QString("function=update_icon;name=ID7;lat=%1;lon=%2;rotate=%4;")
										   .arg(lat)
										   .arg(lon).arg(rot)
										   );

		}

	}
}

void testcontainer::on_pushButton_test_request_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=mark_names;");
	show_message("geomarker::mark_names:"+res);

	QMap<QString,QVariant> mp =  string_to_map(res);
	QString str_prop_vis = "function=props_vis;" ;
	int c = 0;
	foreach (QString key, mp.keys())
	{
		str_prop_vis += QString("name%1=%2;").arg(c++).arg(mp[key].toString());
		res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
											 "function=mark;name="+mp[key].toString());

		show_message("geomarker::mark:"+res);
		res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
													 "function=props;name="+mp[key].toString());
		show_message("geomarker::props:"+res);
	}
	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName, str_prop_vis);
	show_message("geomarker::props_vis:"+res);


}
void testcontainer::on_pushButton_test_cache_clicked()
{
	//Get the address of local cache
	QString res = ui->osmmap->osm_get_local_cache("OSM");
	QMessageBox::information(this,"geomarker::osm_get_local_cache",res);
	//set it to /
	ui->osmmap->osm_set_local_cache("OSM","/OSMCache");
	//Get expire Days
	res = ui->osmmap->osm_get_cache_expire_days("OSM");
	QMessageBox::information(this,"geomarker::osm_get_cache_expire_days",res);
	res = ui->osmmap->osm_set_cache_expire_days("OSM",res.toInt()+1);
	QMessageBox::information(this,"geomarker::osm_get_cache_expire_days",res);
}
void testcontainer::on_pushButton_test_xml_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=load_xml;xml=.//test.xml;");
	QMessageBox::information(this,"geomarker::load_xml",res);
	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=save_xml;xml=.//test.xml;");
	QMessageBox::information(this,"geomarker::save_xml",res);

}
void testcontainer::on_pushButton_test_resource_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=add_resource;name=lena;filename=./lena.png;centerx=32;centery=32;");
	QMessageBox::information(this,"geomarker::add_resource",res);
	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=load_resources;xml=.//resource.xml;");
	QMessageBox::information(this,"geomarker::load_resources",res);
	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=save_resources;xml=.//resource.xml;");
	QMessageBox::information(this,"geomarker::save_resources",res);

}
void testcontainer::on_osmmap_map_event(QMap<QString, QVariant> p)
{
	QList<QStandardItem *> list_newrow;
	list_newrow << new QStandardItem(QString("%1").arg((quint64)ui->osmmap));
	QString message = this->map_to_string(p);

	if (message.contains("MOUSE_MOVE"))
	{
		ui->label_mouseMove->setText(message);
	}
	else
	{
		list_newrow << new QStandardItem(QString("%1").arg(message));
		m_pModel->appendRow(list_newrow);
		while (m_pModel->rowCount()>1024)
			m_pModel->removeRow(0);
		ui->tableView_msg->scrollToBottom();
	}
}
void testcontainer::on_pushButton_test_geo_displayMod_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=set_mod;mod=0;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::set_mod",res);

}

void testcontainer::on_pushButton_test_geo_selectionMod_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=set_mod;mod=1;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::set_mod",res);
}

void testcontainer::on_pushButton_test_geo_selected_marks_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=selected_items;");
	QMessageBox::information(this,"geomarker::selected_items",res);
}

void testcontainer::on_pushButton_test_geo_clear_sel_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=selection_clear;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::selection_clear",res);

}

void testcontainer::on_pushButton_test_geo_del_sel_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=selection_delete;");
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::selection_delete",res);

}
void testcontainer::on_pushButton_default_style_clicked()
{
	QString res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
												 "function=default_style;");
	QMessageBox::information(this,"geomarker::default_style",res);
	res = ui->osmmap->osm_layer_call_function(m_str_markerLayerName,
													"function=set_default_style;" + res);
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker::selection_delete",res);

}
