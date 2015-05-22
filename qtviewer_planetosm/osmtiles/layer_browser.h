#ifndef LAYER_BROSWER_H
#define LAYER_BROSWER_H

#include <QObject>
#include "layer_interface.h"
#include "urlDownloader.h"

class tilesviewer;
class layer_browser :public QObject, public layer_interface
{
	Q_OBJECT
public:
	explicit layer_browser(QObject *parent = 0);
	~layer_browser();
public:
	virtual bool cb_initlayer(QObject  * viewer);
	virtual void cb_finilayer(void * viewer);
	virtual QWidget * cb_create_propWindow();

	virtual void cb_paintEvent( QPainter * pImage );
	virtual bool cb_mousePressEvent ( QMouseEvent * event );
	virtual bool cb_mouseReleaseEvent ( QMouseEvent * event );
	virtual bool cb_mouseMoveEvent(QMouseEvent * event);
	virtual bool cb_wheelEvent ( QWheelEvent * event );
	virtual void cb_resizeEvent ( QResizeEvent * event);
	virtual void cb_levelChanged(int nLevel);
	virtual void cb_update();

public:
	bool cb_isActive() {return true;}
	void cb_setActive(bool at) { }
	bool cb_isVisible() {return true;}
	void cb_setVisible(bool vb) {  }
	QString cb_name() {return m_name;}
	void cb_setName(QString vb) { m_name = vb; }
protected:
	tilesviewer * m_pViewer;
	QString m_name;
private:
	//get single tile from web service
	bool getTileImage(int nLevel,int nX,int nY,QImage & image);
};
#endif // LAYER_BROSWER_H
