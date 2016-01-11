#include "testcontainer.h"
#include "ui_testcontainer.h"
#include <QAxBase>
#include <QDebug>
#include <QMessageBox>
testcontainer::testcontainer(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::testcontainer)
{
	ui->setupUi(this);
	Qt::WindowFlags flg =  this->windowFlags();
	flg |= Qt::WindowMinMaxButtonsHint;
	this->setWindowFlags(flg);

	m_pModel = new QStandardItemModel (0,2,this);
	m_pModel->setHeaderData(0,Qt::Horizontal,tr("ctrl"));
	m_pModel->setHeaderData(1,Qt::Horizontal,tr("msg"));
	ui->tableView_msg->setModel(m_pModel);

	ui->axWidget_map1->setControl("{8FDF97AD-FEFA-44C9-973B-1B66D4C529DF}");
	ui->axWidget_map2->setControl("{8FDF97AD-FEFA-44C9-973B-1B66D4C529DF}");
	connect (ui->axWidget_map1,SIGNAL(evt_Message(QString)),this,SLOT(slot_message(QString)));
	connect (ui->axWidget_map2,SIGNAL(evt_Message(QString)),this,SLOT(slot_message(QString)));
	m_nAnTimer = startTimer(150);
}

testcontainer::~testcontainer()
{
	delete ui;
}
void testcontainer::closeEvent(QCloseEvent *)
{
	QCoreApplication::quit();
}
void testcontainer::slot_message(QString message)
{
	if (message.contains("MOUSE_MOVE"))
	{
		ui->label_mouseMove->setText(message);
	}
	else
	{
		QAxBase * base = qobject_cast<QAxBase *>(sender());
		QList<QStandardItem *> list_newrow;
		list_newrow << new QStandardItem(QString("%1").arg((quint64)base));
		list_newrow << new QStandardItem(QString("%1").arg(message));
		m_pModel->appendRow(list_newrow);
		while (m_pModel->rowCount()>1024)
			m_pModel->removeRow(0);
		ui->tableView_msg->scrollToBottom();
	}

}

void testcontainer::on_pushButton_test_adds_clicked()
{
	//get raw address
	QVariant vt_addr = ui->axWidget_map1->dynamicCall(
				"osm_get_remote_address(QString)",
				"OSM");
	QString str_addr = vt_addr.toString();
	QMessageBox::information(this,"get osm address from LAYER \"OSM\"",str_addr);

	//set address to another value
	ui->axWidget_map1->dynamicCall(
				"osm_set_remote_address(QString,QString)",
				"OSM",
				"http://localhost/osmtile.cgi?level=%1&x=%2&y=%3");

	//get again
	QVariant vt_addr2 = ui->axWidget_map1->dynamicCall(
				"osm_get_remote_address(QString)",
				"OSM");
	QString str_addr2 = vt_addr2.toString();
	QMessageBox::information(this,"get osm address from LAYER \"OSM\"",str_addr2);

	//set address to another value
	ui->axWidget_map1->dynamicCall(
				"osm_set_remote_address(QString,QString)",
				"OSM", str_addr);

	//get again
	QVariant vt_addr3 = ui->axWidget_map1->dynamicCall(
				"osm_get_remote_address(QString)",
				"OSM");
	QString str_addr3 = vt_addr3.toString();
	QMessageBox::information(this,"get osm address from LAYER \"OSM\"",str_addr3);

}
void testcontainer::on_pushButton_test_autodl_clicked()
{
	//get auto download flag
	QVariant vt_addr = ui->axWidget_map1->dynamicCall(
				"osm_get_auto_download(QString)",
				"OSM");
	int n_status = vt_addr.toInt();
	QMessageBox::information(this,"get auto download from LAYER \"OSM\"",QString("status = %1").arg(n_status) );

	//set flag to opposite option
	ui->axWidget_map1->dynamicCall(
				"osm_set_auto_download(QString, int)",
				"OSM" ,
				n_status==0?-1:0);

	//get again
	vt_addr = ui->axWidget_map1->dynamicCall(
				"osm_get_auto_download(QString)",
				"OSM");
	n_status = vt_addr.toInt();
	QMessageBox::information(this,"get auto download from LAYER \"OSM\"",QString("status = %1").arg(n_status) );
}
void testcontainer::on_pushButton_test_navigate_clicked()
{
	//Get curent Level
	QVariant vt_addr = ui->axWidget_map1->dynamicCall("osm_get_level()");
	int n_level = vt_addr.toInt();
	QMessageBox::information(this,"get_level",QString("level = %1").arg(n_level) );

	//Set level to a new value
	ui->axWidget_map1->dynamicCall(
				"osm_set_level(int)",
				n_level<5?++n_level:--n_level);

	//Let's see level again
	vt_addr = ui->axWidget_map1->dynamicCall("osm_get_level()");
	n_level = vt_addr.toInt();
	QMessageBox::information(this,"get_level",QString("level = %1").arg(n_level) );

	//Get map center position
	double lat = ui->axWidget_map1->dynamicCall("osm_get_center_lat()").toDouble();
	double lon = ui->axWidget_map1->dynamicCall("osm_get_center_lon()").toDouble();
	QMessageBox::information(this,"osm_get_center_latlon",QString("lat = %1, lon=%2").arg(lat).arg(lon) );

	//Set a new center position
	lat = 31; lon = 121;
	ui->axWidget_map1->dynamicCall("osm_set_center_pos(double,double)",lat,lon);

	//Get map center position
	lat = ui->axWidget_map1->dynamicCall("osm_get_center_lat()").toDouble();
	lon = ui->axWidget_map1->dynamicCall("osm_get_center_lon()").toDouble();
	QMessageBox::information(this,"osm_get_center_latlon",QString("lat = %1, lon=%2").arg(lat).arg(lon) );
}
void testcontainer::on_pushButton_test_layers_clicked()
{
	//Get Total layers
	QVariant vt_num = ui->axWidget_map1->dynamicCall("osm_layer_get_count()");
	int n_num = vt_num.toInt();

	//Get Layer names
	QString strLayerNames;
	for (int i=0;i<n_num;++i)
	{
		QVariant vt_name = ui->axWidget_map1->dynamicCall("osm_layer_get_name(int)",i);
		QString strname = vt_name.toString();
		strLayerNames += strname + ";";
	}
	QMessageBox::information(this,"layer count",QString("osm_layer_get_count() returns  %1\n%2").arg(n_num).arg(strLayerNames) );

}
void testcontainer::on_pushButton_test_layer_move_clicked()
{
	//Move layers up and down
	ui->axWidget_map1->dynamicCall("osm_layer_move_up(QString)","grid1");
	on_pushButton_test_layers_clicked();
	ui->axWidget_map1->dynamicCall("osm_layer_move_bottom(QString)","grid1");
	on_pushButton_test_layers_clicked();
	ui->axWidget_map1->dynamicCall("osm_layer_move_top(QString)","grid1");
	on_pushButton_test_layers_clicked();
	ui->axWidget_map1->dynamicCall("osm_layer_move_down(QString)","grid1");
	on_pushButton_test_layers_clicked();

	//Set layer's visiblity
	int bv = ui->axWidget_map1->dynamicCall("osm_layer_get_visiable(QString)","grid1").toInt();
	QMessageBox::information(this,"visibility",QString("osm_layer_get_visiable(\"grid1\") returns  %1").arg(bv));
	bv = ui->axWidget_map1->dynamicCall("osm_layer_set_visiable(QString, int)","grid1", bv==0?-1:0).toInt();
	QMessageBox::information(this,"visibility",QString("osm_layer_set_visiable(\"grid1\") returns  %1").arg(bv));
	bv = ui->axWidget_map1->dynamicCall("osm_layer_set_visiable(QString, int)","grid1", bv==0?-1:0).toInt();
	QMessageBox::information(this,"visibility",QString("osm_layer_set_visiable(\"grid1\") returns  %1").arg(bv));

	//Set layer's activity
	int av = ui->axWidget_map1->dynamicCall("osm_layer_get_active(QString)","OSM").toInt();
	QMessageBox::information(this,"active",QString("osm_layer_get_active(\"OSM\") returns  %1").arg(av));
	av = ui->axWidget_map1->dynamicCall("osm_layer_set_active(QString,int)","OSM",av==0?-1:0).toInt();
	QMessageBox::information(this,"active",QString("osm_layer_set_active(\"OSM\") returns  %1").arg(av));
	av = ui->axWidget_map1->dynamicCall("osm_layer_set_active(QString,int)","OSM",av==0?-1:0).toInt();
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
	QString res = ui->axWidget_map1->dynamicCall(
				"osm_layer_call_function(QString,QString)",
				"grid1",
				"function=get_ruler_status;").toString();
	QMessageBox::information(this,"grid1::get_ruler_status",res);

	//Check result
	QMap<QString, QVariant> mres = string_to_map(res);
	if (mres["status"].toInt())
	{
		res = ui->axWidget_map1->dynamicCall(
					"osm_layer_call_function(QString,QString)",
					"grid1",
					"function=set_ruler_status;status=0;").toString();
		QMessageBox::information(this,"grid1::set_ruler_status to false, you can call get_polygon to get polygon strings..",res);
	}
	else
	{
		res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)",
											 "grid1",
											 "function=set_ruler_status;status=-1;").toString();
		QMessageBox::information(this,"grid1::set_ruler_status to true, you can draw polygons on map using mouse lbutton for begin and rbutton for end.",res);
	}

}
void testcontainer::on_pushButton_test_grid_getPolygon_clicked()
{
	//Get current ploygon lla.
	QString res = ui->axWidget_map1->dynamicCall(
				"osm_layer_call_function(QString,QString)",
				"grid1",
				"function=get_polygon;").toString();
	QMessageBox::information(this,"grid1::get_polygon",res);

}
void testcontainer::on_pushButton_test_mark_clicked()
{
	QString res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=delete_marks;name0=ID3;name1=ID4;").toString();
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::delete_marks",res);

	res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=update_point;name=ID1;type=1;"
												 "lat=31.3737;lon=121.3783474;"
												 "style_pen=2;color_pen=0,0,255,128;width_pen=3;"
												 "style_brush=1;color_brush=0,255,0,128;"
												 "color_label=0,0,255,96;weight_label=99;size_label=12;"
												 "width=16;height=20;").toString();
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::update_point",res);


	res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=update_props;name=ID1;"
												 "LABEL=Shanghai;EXPRESS=Shunfeng;Pero=IMMEDIATE;"
												 "CheckTime=2014-12-31 23:11:27;"
												 "From=Shanghai;To=Beijing").toString();
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::update_point",res);


	res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=update_point;name=ID2;type=2;"
												 "lat=40.3737;lon=111.34347;"
												 "style_pen=3;color_pen=0,255,0,128;"
												 "width_pen=2;style_brush=3;color_brush=255,128,0,128;"
												 "width=12;height=12;").toString();

	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::update_point",res);

	res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=update_props;name=ID2;"
												 "LABEL=Neimeng;EXPRESS=YunDa;Pero=NORMAL;"
												 "CheckTime=2014-12-30 07:18:32;"
												 "From=Huhehaote;To=YinChuan").toString();
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::update_point",res);

}
void testcontainer::on_pushButton_test_line_clicked()
{
	QString res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=delete_marks;name0=ID1;name1=ID2;name2=ID4;").toString();
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::delete_marks",res);

	res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=update_line;name=ID3;type=3;"
												 "lat0=31.3737;lon0=121.3783474;"
												 "lat1=40.3737;lon1=111.34347;"
												 "style_pen=4;color_pen=255,0,0,96;"
												 "width_pen=2;").toString();
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::update_line",res);


	res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=update_props;name=ID3;"
												 "LABEL=HighWay;").toString();
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::update_point",res);
}

void testcontainer::on_pushButton_test_polygon_clicked()
{
	QString res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=delete_marks;name0=ID1;name1=ID2;name2=ID3;").toString();
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::delete_marks",res);

	res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=update_polygon;name=ID4;type=4;"
												 "lat0=12.2;lon0=67.3;"
												 "lat1=14.3;lon1=62.8;"
												 "lat2=22.7;lon2=66.5;"
												 "lat3=11.5;lon3=72.2;"
												 "lat4=10.8;lon4=69.4;"
												 "style_pen=2;color_pen=0,0,255,128;"
												 "width_pen=3;style_brush=1;color_brush=0,255,0,128;").toString();
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::update_polygon",res);

	res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
												 "function=update_props;name=ID4;"
												 "LABEL=Region;").toString();
	if (res.contains("error"))
		QMessageBox::information(this,"geomarker1::update_point",res);
}

void testcontainer::timerEvent(QTimerEvent * e)
{
	if (e->timerId()==m_nAnTimer)
	{
		static double tposlat = 31, tposlon=121;
		tposlat += sin(tposlon * 3.14/180.0)/5+0.1;
		tposlon -= cos(tposlat * 3.14/180.0)/5+0.1;
		if (tposlat > 85 ) tposlat = -85;
		if (tposlat < -85 ) tposlat = 85;
		if (tposlon < -180 ) tposlon = 180;
		if (tposlon > 180 ) tposlon = -180;
		QString res = ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
													 "function=exists;name=ID1;").toString();
		QMap<QString, QVariant> mres = string_to_map(res);
		if (mres["return"].toInt())
		{
			ui->axWidget_map1->dynamicCall("osm_layer_call_function(QString,QString)","geomarker1",
										   QString("function=update_point;name=ID1;type=1;lat=%1;lon=%2;")
										   .arg(tposlat)
										   .arg(tposlon)
										   ).toString();

		}

	}
}
