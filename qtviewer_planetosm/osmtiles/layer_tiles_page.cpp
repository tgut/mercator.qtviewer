#include "layer_tiles_page.h"
#include "ui_layer_tiles_page.h"
#include "layer_tiles.h"
#include <QSettings>
#include <QCoreApplication>
#include <QFileDialog>
layer_tiles_page::layer_tiles_page(layer_tiles * layer,QWidget *parent) :
	QWidget(parent),
	m_pLayer(layer),
	ui(new Ui::layer_tiles_page)
{
	ui->setupUi(this);
	//Get Cache Address
	QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
	QString strServerURL = settings.value(QString("settings/ServerURL_%1").arg(layer->cb_name()),"http://localhost/osm_tiles2/%1/%2/%3.png").toString();
	QString strLocalCache = settings.value(QString("settings/LocalCache_%1").arg(layer->cb_name()), QCoreApplication::applicationDirPath() +"/Cache").toString();
	ui->lineEdit->setText(strLocalCache);
	ui->plainTextEdit->setPlainText( strServerURL);
}

layer_tiles_page::~layer_tiles_page()
{
	delete ui;
}
void layer_tiles_page::on_toolButton_browser_clicked()
{
	QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
	QString strLocalCache = settings.value(QString("settings/LocalCache_%1").arg(m_pLayer->cb_name()), QCoreApplication::applicationDirPath() +"/Cache").toString();
	QString strFolder = QFileDialog::getExistingDirectory(this,tr("Select the local cache"),strLocalCache,QFileDialog::ShowDirsOnly);
	if (strFolder.length())
		ui->lineEdit->setText(strFolder);
}

void layer_tiles_page::on_pushButton_apply_clicked()
{
	m_pLayer->setLocalCache(ui->lineEdit->text());
	m_pLayer->setServerUrl(ui->plainTextEdit->toPlainText());
	m_pLayer->UpdateLayer();
}
