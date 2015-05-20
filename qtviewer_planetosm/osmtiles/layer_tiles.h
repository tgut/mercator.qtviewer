#ifndef LAYER_TILES_H
#define LAYER_TILES_H

#include <QObject>
#include "layer_interface.h"
#include "urlDownloader.h"
#include "layer_tiles_page.h"
class tilesviewer;
class layer_tiles :public QObject,public layer_interface
{
	Q_OBJECT
	friend class layer_tiles_page;
public:
	explicit layer_tiles(QObject *parent = 0);
	~layer_tiles();
	QString serverUrl() {return m_strServerURL; }
	QString localCache() {return m_strLocalCache; }
	bool isConnected(){return m_bconnected;}
	void connectToTilesServer(bool bconnected);
	void UpdateLayer();
public:
	virtual bool cb_initlayer(QObject  * viewer);
	virtual void cb_finilayer(void * viewer);
	virtual QWidget * cb_create_propWindow();
	virtual void cb_paintEvent( QPainter * pImage  );
	virtual bool cb_mousePressEvent ( QMouseEvent * event );
	virtual bool cb_mouseReleaseEvent ( QMouseEvent * event );
	virtual bool cb_mouseMoveEvent(QMouseEvent * event);
	virtual bool cb_wheelEvent ( QWheelEvent * event );
	virtual void cb_resizeEvent ( QResizeEvent * event);
	virtual void cb_levelChanged(int nLevel);
	virtual void cb_update();

public:
	bool cb_isActive() {return m_bActive;}
	void cb_setActive(bool at) { m_bActive = at; cb_update();}
	bool cb_isVisible() {return m_bVisible;}
	void cb_setVisible(bool vb) { m_bVisible = vb; cb_update(); }
	QString cb_name() {return m_name;}
	void cb_setName(QString vb) { m_name = vb; }
protected:
	void setServerUrl(QString url);
	void setLocalCache(QString cache);

	tilesviewer * m_pViewer;
	layer_tiles_page * m_propPage;
	bool m_bActive;
	bool m_bVisible;
	quint8 m_nAlpha;
	QString m_name;
private:
	bool m_bconnected;
	QString m_strLocalCache;
	QString m_strServerURL;
	//The download tools
	urlDownloader * m_downloader;
	QThread * m_downloadThread;
	//drag
	int m_nStartPosX;
	int m_nStartPosY;
private:
	//get single tile from web service
	bool getTileImage(int nLevel,int nX,int nY,QImage & image);
	//regisit images to web service
	bool RegImages(int nLeft,int nRight,int nTop,int nBottom,int nLevel);
};

#endif // LAYER_TILES_H
