#ifndef LAYER_BROSWER_H
#define LAYER_BROSWER_H

#include <QObject>
#include "layer_interface.h"
#include "urlDownloader.h"
namespace QTVOSM{
	class tilesviewer;
	/*! this class defines a tile layer for browser-view (Bird-eye view)
	 *
	 * \class	layer_browser layer_browser.h "qtviewer_planetosm/osmtiles/layer_browser.h"
	 * \author	goldenhawking	\date	2015-12-11
	*/
	class layer_browser :public QObject, public layer_interface
	{
		Q_OBJECT
	public:
		explicit layer_browser(QObject *parent = 0);
		~layer_browser();
		virtual QWidget *	get_propWindow() {return 0;}
	public:
		virtual layer_interface * load_initial_plugin(QString strSLibPath,viewer_interface  * viewer);
		virtual QWidget * load_prop_window();

		virtual void cb_paintEvent( QPainter * pImage );
		virtual bool cb_mousePressEvent ( QMouseEvent * event );

	public:
		bool is_active() {return true;}
		void set_active(bool ) { }
		bool is_visible() {return true;}
		void set_visible(bool ) {  }
		QString get_name() {return m_name;}
		void set_name(QString vb) { m_name = vb; }
	protected:
		viewer_interface * m_pViewer;
		QString m_name;
	private:
		//get single tile from web service
		bool getTileImage(int nLevel,int nX,int nY,QImage & image);
	};
}
#endif // LAYER_BROSWER_H
