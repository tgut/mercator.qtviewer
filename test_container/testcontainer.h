#ifndef TESTCONTAINER_H
#define TESTCONTAINER_H

#include <QDialog>
#include <QStandardItemModel>
namespace Ui {
	class testcontainer;
}

class testcontainer : public QDialog
{
	Q_OBJECT
private:
	QStandardItemModel * m_pModel;
public:
	explicit testcontainer(QWidget *parent = 0);
	~testcontainer();
protected:
	void closeEvent(QCloseEvent *);
private:
	Ui::testcontainer *ui;
protected slots:
	void slot_message(QString ,QString ,double,double,QString);
};

#endif // TESTCONTAINER_H
