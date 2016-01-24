#ifndef LAYER_TILES_H
#define LAYER_TILES_H

#include <QObject>
#include <QVector>
#include "layer_interface.h"
#include "urlDownloader.h"
#include "layer_tiles_page.h"
namespace QTVOSM{
	class tilesviewer;
	/*! \brief	layer_tiles is a basic Backgroud Layer, with is_exclusive = true.
	 * This means only ONE instances of layer_tiles could be active at sametime
	 *
	 * \class	layer_tiles layer_tiles.h "qtviewer_planetosm/osmtiles/layer_tiles.h"
	 * \author	goldenhawking	\date	2015-12-11
	*/
	class layer_tiles :public QObject,public layer_interface
	{
		Q_OBJECT
		friend class layer_tiles_page;
	public:
		explicit layer_tiles(QObject *parent = 0);
		~layer_tiles();
		QWidget *	get_propWindow(){return m_propPage;}
		void load_retranslate_UI(){m_propPage->reTransUI();}

		QString serverUrl() {return m_strServerURL; }
		QString localCache() {return m_strLocalCache; }
		int cacheExpireDays() {return m_nCacheExpireDays; }

		bool isConnected(){return m_bconnected;}
		void connectToTilesServer(bool bconnected);
		void UpdateLayer();
	public:
		virtual layer_interface * load_initial_plugin(QString strSLibPath,viewer_interface  * viewer);
		virtual QWidget * load_prop_window();
		virtual void cb_paintEvent( QPainter * pImage  );
		virtual bool cb_mousePressEvent ( QMouseEvent * event );
		virtual bool cb_mouseReleaseEvent ( QMouseEvent * event );
		virtual bool cb_mouseMoveEvent(QMouseEvent * event);
		virtual bool cb_wheelEvent ( QWheelEvent * event );
		virtual void cb_resizeEvent ( QResizeEvent * event);
		virtual void cb_levelChanged(int nLevel);

	public:
		bool is_active() {return m_bActive;}
		bool is_visible() {return m_bVisible;}
		void set_visible(bool vb) { m_bVisible = vb;  }
		QString get_name() {return m_name;}
		void set_name(QString vb) { m_name = vb; }
		/*! OSM tiles has a responsability to maintain drag and mouse click,
			It's exclusive, there should be only one layer_tiles active
		*/
		bool is_exclusive(){return true;}
		void set_active(bool at) { m_bActive = at;}
	protected:
		viewer_interface * m_pViewer;
		layer_tiles_page * m_propPage;
		bool m_bActive;
		bool m_bVisible;
		quint8 m_nAlpha;
		QString m_name;
	private:
		bool m_bconnected;
		int	m_nCacheExpireDays;
		QString m_strLocalCache;
		QString m_strServerURL;
		//The download tools
		urlDownloader * m_downloader;
		QThread * m_downloadThread;
		//drag
		int m_nStartPosX;
		int m_nStartPosY;
	private:
		//! get single tile from web service
		bool getTileImage(int nLevel,int nX,int nY,QImage & image);
		//! regisit images to web service
		bool RegImages(int nX, int nY,int nLevel);
	protected slots:
		void updateViewer();
	public slots:
		void setServerUrl(QString url);
		void setLocalCache(QString cache);
		void setCacheExpireDays(int nCacheExpireDays);
	signals:
		void connected(bool);
		void svrurlChanged(QString);
		void cacheChanged(QString);
		void cacheExpireChanged(int);
	};
}
#endif // LAYER_TILES_H
