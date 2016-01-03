#include "testcontainer.h"
#include "ui_testcontainer.h"
#include <QAxBase>
#include <QDebug>
testcontainer::testcontainer(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::testcontainer)
{
	ui->setupUi(this);
	Qt::WindowFlags flg =  this->windowFlags();
	flg |= Qt::WindowMinMaxButtonsHint;
	this->setWindowFlags(flg);

	m_pModel = new QStandardItemModel (0,6,this);
	m_pModel->setHeaderData(0,Qt::Horizontal,tr("ctrl"));
	m_pModel->setHeaderData(1,Qt::Horizontal,tr("source"));
	m_pModel->setHeaderData(2,Qt::Horizontal,tr("msg"));
	m_pModel->setHeaderData(3,Qt::Horizontal,tr("lat"));
	m_pModel->setHeaderData(4,Qt::Horizontal,tr("lon"));
	m_pModel->setHeaderData(5,Qt::Horizontal,tr("props"));

	ui->tableView_msg->setModel(m_pModel);

	ui->axWidget_map1->setControl("{8FDF97AD-FEFA-44C9-973B-1B66D4C529DF}");
	ui->axWidget_map1->dynamicCall("setTileAddress(QString)","http://c.tile.openstreetmap.org/%1/%2/%3.png");
	ui->axWidget_map1->dynamicCall("ConnectToServer()");

	ui->axWidget_map2->setControl("{8FDF97AD-FEFA-44C9-973B-1B66D4C529DF}");
	ui->axWidget_map2->dynamicCall("setTileAddress(QString)","http://c.tile.openstreetmap.org/%1/%2/%3.png");
	ui->axWidget_map2->dynamicCall("ConnectToServer()");

	connect (ui->axWidget_map1,SIGNAL(evt_Message(QString ,QString ,double,double,QString)),this,SLOT(slot_message(QString ,QString ,double,double,QString)));
	connect (ui->axWidget_map2,SIGNAL(evt_Message(QString ,QString ,double,double,QString)),this,SLOT(slot_message(QString ,QString ,double,double,QString)));
}

testcontainer::~testcontainer()
{
	delete ui;
}
void testcontainer::closeEvent(QCloseEvent *)
{
	QCoreApplication::quit();
}
void testcontainer::slot_message(QString src ,QString msg ,double lat,double lon,QString ext)
{
	QAxBase * base = qobject_cast<QAxBase *>(sender());
	QList<QStandardItem *> list_newrow;
	list_newrow << new QStandardItem(QString("%1").arg((quint64)base));
	list_newrow << new QStandardItem(QString("%1").arg(src));
	list_newrow << new QStandardItem(QString("%1").arg(msg));
	list_newrow << new QStandardItem(QString("%1").arg(lat));
	list_newrow << new QStandardItem(QString("%1").arg(lon));
	list_newrow << new QStandardItem(QString("%1").arg(ext));
	//if (base)
	//	qDebug()<<(quint64)base<<src <<" send "<<msg<< " lat="<<lat<<", lon="<<lon<<" extmsg="<<ext;

	m_pModel->appendRow(list_newrow);
	while (m_pModel->rowCount()>1024)
		m_pModel->removeRow(0);
	ui->tableView_msg->scrollToBottom();
}
