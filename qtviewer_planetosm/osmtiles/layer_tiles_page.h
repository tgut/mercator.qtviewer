#ifndef LAYER_TILES_PAGE_H
#define LAYER_TILES_PAGE_H

#include <QWidget>

namespace Ui {
	class layer_tiles_page;
}
class layer_tiles;
class layer_tiles_page : public QWidget
{
	Q_OBJECT

public:
	explicit layer_tiles_page(layer_tiles * layer,QWidget *parent = 0);
	~layer_tiles_page();

private:
	Ui::layer_tiles_page *ui;
	layer_tiles * m_pLayer;
public slots:
	void on_toolButton_browser_clicked();
	void on_pushButton_apply_clicked();
};

#endif // LAYER_TILES_PAGE_H
