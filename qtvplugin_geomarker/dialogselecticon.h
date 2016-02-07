#ifndef DIALOGSELECTICON_H
#define DIALOGSELECTICON_H

#include <QDialog>
#include "geographicspixmapitem.h"
namespace Ui {
	class DialogSelectIcon;
}

class DialogSelectIcon : public QDialog
{
	Q_OBJECT

public:
	explicit DialogSelectIcon(QWidget *parent = 0);
	~DialogSelectIcon();
	QString iniFileName;
	QTVP_GEOMARKER::tag_icon m_icon;
private:
	Ui::DialogSelectIcon *ui;
protected slots:
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();
	void on_toolButton_browser_clicked();
};

#endif // DIALOGSELECTICON_H
