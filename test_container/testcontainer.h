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
	QString map_to_string(const QMap<QString, QVariant> & m);
	QMap<QString, QVariant> string_to_map(const QString & s);
public:
	explicit testcontainer(QWidget *parent = 0);
	~testcontainer();
protected:
	void closeEvent(QCloseEvent *);
private:
	Ui::testcontainer *ui;
protected slots:
	void slot_message(QString);
	void on_pushButton_test_adds_clicked();
	void on_pushButton_test_autodl_clicked();
	void on_pushButton_test_navigate_clicked();
	void on_pushButton_test_layers_clicked();
	void on_pushButton_test_layer_move_clicked();
	void on_pushButton_test_grid_enable_clicked();
	void on_pushButton_test_grid_getRegion_clicked();
};

#endif // TESTCONTAINER_H
