#include "layer_tiles_page.h"
#include "ui_layer_tiles_page.h"
#include "layer_tiles.h"
#include <QSettings>
#include <QCoreApplication>
#include <QFileDialog>
#include <QDebug>
#include <QCheckBox>
namespace QTVOSM{
	layer_tiles_page::layer_tiles_page(layer_tiles * layer,QWidget *parent) :
		QWidget(parent),
		m_pLayer(layer),
		ui(new Ui::layer_tiles_page),
		m_nTimerID(startTimer(1000))
	{
		ui->setupUi(this);
		//Get Cache Address
		QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
		QString strServerURL = settings.value(QString("settings/ServerURL_%1").arg(layer->get_name()),"http://c.tile.openstreetmap.org/%1/%2/%3.png").toString();
		QString strLocalCache = settings.value(QString("settings/LocalCache_%1").arg(layer->get_name()), QCoreApplication::applicationDirPath() +"/OSMCache").toString();
		ui->lineEdit->setText(strLocalCache);
		ui->lineEdit_addressUrl->setText(strServerURL);
		this->setWindowTitle(layer->get_name());
		//the pending tasks model
		m_pPendingTasksModel = new QStandardItemModel(0,3,this);
		m_pPendingTasksModel->setHeaderData(0,Qt::Horizontal,tr("url"));
		m_pPendingTasksModel->setHeaderData(1,Qt::Horizontal,tr("destin dir"));
		m_pPendingTasksModel->setHeaderData(2,Qt::Horizontal,tr("filename"));
		ui->tableView_pendingTasks->setModel(m_pPendingTasksModel);

		connect (layer, &layer_tiles::connected ,this->ui->checkBox_connect, &QCheckBox::setChecked);
		connect (layer, &layer_tiles::svrurlChanged ,this->ui->lineEdit_addressUrl, &QLineEdit::setText);
		ui->checkBox_connect->setChecked(layer->isConnected());
	}
	void layer_tiles_page::reTransUI()
	{
		ui->retranslateUi(this);
	}

	layer_tiles_page::~layer_tiles_page()
	{
		delete ui;
	}
	void layer_tiles_page::on_toolButton_browser_clicked()
	{
		QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
		QString strLocalCache = settings.value(QString("settings/LocalCache_%1").arg(m_pLayer->get_name()), QCoreApplication::applicationDirPath() +"/OSMCache").toString();
		QString strFolder = QFileDialog::getExistingDirectory(this,tr("Select the local cache"),strLocalCache,QFileDialog::ShowDirsOnly);
		if (strFolder.length())
			ui->lineEdit->setText(strFolder);
	}

	void layer_tiles_page::on_pushButton_apply_clicked()
	{
		m_pLayer->setLocalCache(ui->lineEdit->text());
		m_pLayer->setServerUrl(ui->lineEdit_addressUrl->text());
		m_pLayer->UpdateLayer();
	}
	void  layer_tiles_page::timerEvent(QTimerEvent * e)
	{
		if (e->timerId()==m_nTimerID)
		{
			killTimer(m_nTimerID);
			QVector<tag_download_tasks> vec_tk = m_pLayer->current_tasks();
			//qDebug()<<vec_tk.size();
			foreach (const tag_download_tasks & t, vec_tk)
			{
				QList<QStandardItem *> row;
				row<<new QStandardItem(t.str_url);
				row<<new QStandardItem(t.str_destinDir);
				row<<new QStandardItem(t.str_destinFile);
				m_pPendingTasksModel->appendRow(row);
			}//end for each task
			if (m_pPendingTasksModel->rowCount()>256)
				m_pPendingTasksModel->removeRows(0,128);
			m_nTimerID =  startTimer(1000);
		}//end if timer

	}
	void layer_tiles_page::on_checkBox_connect_clicked(bool ps)
	{
		m_pLayer->connectToTilesServer(ps);
	}
}
