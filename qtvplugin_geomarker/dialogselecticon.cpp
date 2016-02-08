#include "dialogselecticon.h"
#include "ui_dialogselecticon.h"
#include <QSettings>
#include <QFileDialog>
#include <QPixmap>
#include <QMessageBox>
#include <QFileInfo>
DialogSelectIcon::DialogSelectIcon(QString inifile,QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogSelectIcon),
	iniFileName(inifile)
{
	ui->setupUi(this);
	QSettings settings(iniFileName,QSettings::IniFormat);
	ui->lineEdit_filename->setText(settings.value("IconSel/lineEdit_filename","./1.png").toString());
	ui->lineEdit_name->setText(settings.value("IconSel/lineEdit_name","1").toString());
	ui->lineEdit_centx->setText(settings.value("IconSel/lineEdit_centx","0").toString());
	ui->lineEdit_centy->setText(settings.value("IconSel/lineEdit_centy","0").toString());
}

DialogSelectIcon::~DialogSelectIcon()
{
	delete ui;
}

void DialogSelectIcon::on_pushButton_ok_clicked()
{
	QSettings settings(iniFileName,QSettings::IniFormat);
	settings.setValue("IconSel/lineEdit_filename",ui->lineEdit_filename->text());
	settings.setValue("IconSel/lineEdit_name",ui->lineEdit_name->text());
	settings.setValue("IconSel/lineEdit_centx",ui->lineEdit_centx->text());
	settings.setValue("IconSel/lineEdit_centy",ui->lineEdit_centy->text());

	m_icon.centerx = ui->lineEdit_centx->text().toInt();
	m_icon.centery = ui->lineEdit_centy->text().toInt();
	m_icon.filename = ui->lineEdit_filename->text();
	m_icon.name = ui->lineEdit_name->text();
	if (m_icon.icon.load(m_icon.filename))
		accept();
	else
	{
		QMessageBox::information(this,tr("Error open file"),m_icon.filename);
		reject();
	}
}

void DialogSelectIcon::on_pushButton_cancel_clicked()
{
	reject();
}

void DialogSelectIcon::on_toolButton_browser_clicked()
{
	QSettings settings(iniFileName,QSettings::IniFormat);
	QString strLastSaveImgDir = settings.value("IconSel/last_save_img_dir","./").toString();
	QString newfm = QFileDialog::getOpenFileName(this,tr("load from image"),strLastSaveImgDir,
								 "iamges (*.png;*.jpg;*.jpeg;*.ico;*.bmp;*.tiff);;All files(*.*)"
								 );
	if (newfm.size()>2)
	{
		QPixmap mp;
		if (true==mp.load(newfm))
		{
			ui->lineEdit_filename->setText(newfm);
			QFileInfo info(newfm);
			ui->lineEdit_name->setText(info.completeBaseName());
		}
		else
			QMessageBox::information(this,tr("Error open file"),newfm);
	}
}
