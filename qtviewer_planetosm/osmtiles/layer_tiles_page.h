#ifndef LAYER_TILES_PAGE_H
#define LAYER_TILES_PAGE_H

#include <QWidget>
#include <QStandardItemModel>
namespace Ui {
	class layer_tiles_page;
}
namespace QTVOSM{
	class layer_tiles;
	/*!
	 \brief	layer_tiles_page is the class of proper pages for layer_tiles

	 \class	layer_tiles_page layer_tiles_page.h "qtviewer_planetosm/osmtiles/layer_tiles_page.h"
	 \author	goldenhawking	\date	2015-12-11
	*/
	class layer_tiles_page : public QWidget
	{
		Q_OBJECT

	public:
		explicit layer_tiles_page(layer_tiles * layer,QWidget *parent = 0);
		~layer_tiles_page();
		//re-translat
		void reTransUI();
	private:
		Ui::layer_tiles_page *ui ;
		layer_tiles * m_pLayer ;
		QStandardItemModel * m_pPendingTasksModel;
	public slots:
		void slot_message(QString);
	protected slots:
		void on_toolButton_browser_clicked();
		void on_pushButton_apply_clicked();
		void on_checkBox_connect_clicked(bool);
	};
}
#endif // LAYER_TILES_PAGE_H
