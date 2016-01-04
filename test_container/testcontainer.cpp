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
	QAxBase * base = qobject_cast<QAxBase *>(sender());
	QList<QStandardItem *> list_newrow;
	list_newrow << new QStandardItem(QString("%1").arg((quint64)base));
	list_newrow << new QStandardItem(QString("%1").arg(message));
	m_pModel->appendRow(list_newrow);
	while (m_pModel->rowCount()>1024)
		m_pModel->removeRow(0);
	ui->tableView_msg->scrollToBottom();
}

void testcontainer::on_pushButton_test_adds_clicked()
{
	//get raw address
	QVariant vt_addr = ui->axWidget_map1->dynamicCall("osm_get_remote_address(QString)","OSM");
	QString str_addr = vt_addr.toString();
	QMessageBox::information(this,"get osm address from LAYER \"OSM\"",str_addr);

	//set address to another value
	ui->axWidget_map1->dynamicCall("osm_set_remote_address(QString,QString)","OSM", "http://localhost/osmtile.cgi?level=%1&x=%2&y=%3");

	//get again
	QVariant vt_addr2 = ui->axWidget_map1->dynamicCall("osm_get_remote_address(QString)","OSM");
	QString str_addr2 = vt_addr2.toString();
	QMessageBox::information(this,"get osm address from LAYER \"OSM\"",str_addr2);

	//set address to another value
	ui->axWidget_map1->dynamicCall("osm_set_remote_address(QString,QString)","OSM", str_addr);

	//get again
	QVariant vt_addr3 = ui->axWidget_map1->dynamicCall("osm_get_remote_address(QString)","OSM");
	QString str_addr3 = vt_addr3.toString();
	QMessageBox::information(this,"get osm address from LAYER \"OSM\"",str_addr3);

}
void testcontainer::on_pushButton_test_autodl_clicked()
{
	//get raw address
	QVariant vt_addr = ui->axWidget_map1->dynamicCall("osm_get_auto_download(QString)","OSM");
	int n_status = vt_addr.toInt();
	QMessageBox::information(this,"get auto download from LAYER \"OSM\"",QString("status = %1").arg(n_status) );

	ui->axWidget_map1->dynamicCall("osm_set_auto_download(QString, int)","OSM" , n_status==0?-1:0);
	vt_addr = ui->axWidget_map1->dynamicCall("osm_get_auto_download(QString)","OSM");
	n_status = vt_addr.toInt();
	QMessageBox::information(this,"get auto download from LAYER \"OSM\"",QString("status = %1").arg(n_status) );
}
void testcontainer::on_pushButton_test_navigate_clicked()
{
	QVariant vt_addr = ui->axWidget_map1->dynamicCall("osm_get_level()");
	int n_level = vt_addr.toInt();
	QMessageBox::information(this,"get_level",QString("level = %1").arg(n_level) );

	ui->axWidget_map1->dynamicCall("osm_set_level(int)",n_level<5?++n_level:--n_level);

	vt_addr = ui->axWidget_map1->dynamicCall("osm_get_level()");
	n_level = vt_addr.toInt();
	QMessageBox::information(this,"get_level",QString("level = %1").arg(n_level) );

	double lat = ui->axWidget_map1->dynamicCall("osm_get_center_lat()").toDouble();
	double lon = ui->axWidget_map1->dynamicCall("osm_get_center_lon()").toDouble();
	QMessageBox::information(this,"osm_get_center_latlon",QString("lat = %1, lon=%2").arg(lat).arg(lon) );

	lat = 31; lon = 121;
	ui->axWidget_map1->dynamicCall("osm_set_center_pos(double,double)",lat,lon);
	lat = ui->axWidget_map1->dynamicCall("osm_get_center_lat()").toDouble();
	lon = ui->axWidget_map1->dynamicCall("osm_get_center_lon()").toDouble();
	QMessageBox::information(this,"osm_get_center_latlon",QString("lat = %1, lon=%2").arg(lat).arg(lon) );
}
void testcontainer::on_pushButton_test_layers_clicked()
{
	QVariant vt_num = ui->axWidget_map1->dynamicCall("osm_layer_get_count()");
	int n_num = vt_num.toInt();
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
	ui->axWidget_map1->dynamicCall("osm_layer_move_up(QString)","grid1");
	on_pushButton_test_layers_clicked();
	ui->axWidget_map1->dynamicCall("osm_layer_move_bottom(QString)","grid1");
	on_pushButton_test_layers_clicked();
	ui->axWidget_map1->dynamicCall("osm_layer_move_top(QString)","grid1");
	on_pushButton_test_layers_clicked();
	ui->axWidget_map1->dynamicCall("osm_layer_move_down(QString)","grid1");
	on_pushButton_test_layers_clicked();

	int bv = ui->axWidget_map1->dynamicCall("osm_layer_get_visiable(QString)","grid1").toInt();
	QMessageBox::information(this,"visibility",QString("osm_layer_get_visiable(\"grid1\") returns  %1").arg(bv));
	bv = ui->axWidget_map1->dynamicCall("osm_layer_set_visiable(QString, int)","grid1", bv==0?-1:0).toInt();
	QMessageBox::information(this,"visibility",QString("osm_layer_set_visiable(\"grid1\") returns  %1").arg(bv));
	bv = ui->axWidget_map1->dynamicCall("osm_layer_set_visiable(QString, int)","grid1", bv==0?-1:0).toInt();
	QMessageBox::information(this,"visibility",QString("osm_layer_set_visiable(\"grid1\") returns  %1").arg(bv));

	int av = ui->axWidget_map1->dynamicCall("osm_layer_get_active(QString)","OSM").toInt();
	QMessageBox::information(this,"active",QString("osm_layer_get_active(\"OSM\") returns  %1").arg(av));
	av = ui->axWidget_map1->dynamicCall("osm_layer_set_active(QString,int)","OSM",av==0?-1:0).toInt();
	QMessageBox::information(this,"active",QString("osm_layer_set_active(\"OSM\") returns  %1").arg(av));
	av = ui->axWidget_map1->dynamicCall("osm_layer_set_active(QString,int)","OSM",av==0?-1:0).toInt();
	QMessageBox::information(this,"active",QString("osm_layer_set_active(\"OSM\") returns  %1").arg(av));
}
